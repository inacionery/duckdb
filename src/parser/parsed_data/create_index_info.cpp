#include "duckdb/parser/parsed_data/create_index_info.hpp"

#include "duckdb/common/field_writer.hpp"

namespace duckdb {

unique_ptr<CreateInfo> CreateIndexInfo::Copy() const {
	auto result = make_uniq<CreateIndexInfo>();
	CopyProperties(*result);

	result->index_type = index_type;
	result->index_name = index_name;
	result->constraint_type = constraint_type;
	result->table = table;
	for (auto &expr : expressions) {
		result->expressions.push_back(expr->Copy());
	}
	for (auto &expr : parsed_expressions) {
		result->parsed_expressions.push_back(expr->Copy());
	}

	result->scan_types = scan_types;
	result->names = names;
	result->column_ids = column_ids;
	return std::move(result);
}

void CreateIndexInfo::SerializeInternal(Serializer &serializer) const {
	FieldWriter writer(serializer);
	writer.WriteField(index_type);
	writer.WriteString(table);
	writer.WriteString(index_name);
	writer.WriteField(constraint_type);

	writer.WriteSerializableList<ParsedExpression>(parsed_expressions);

	writer.WriteRegularSerializableList(scan_types);
	writer.WriteList<string>(names);
	writer.WriteList<column_t>(column_ids);
	writer.Finalize();
}

unique_ptr<CreateIndexInfo> CreateIndexInfo::Deserialize(Deserializer &deserializer) {
	auto result = make_uniq<CreateIndexInfo>();
	result->DeserializeBase(deserializer);

	FieldReader reader(deserializer);
	result->index_type = reader.ReadRequired<IndexType>();
	result->table = reader.ReadRequired<string>();
	result->index_name = reader.ReadRequired<string>();
	result->constraint_type = reader.ReadRequired<IndexConstraintType>();

	result->parsed_expressions = reader.ReadRequiredSerializableList<ParsedExpression>();

	result->scan_types = reader.ReadRequiredSerializableList<LogicalType, LogicalType>();
	result->names = reader.ReadRequiredList<string>();
	result->column_ids = reader.ReadRequiredList<column_t>();
	reader.Finalize();
	return result;
}
} // namespace duckdb
