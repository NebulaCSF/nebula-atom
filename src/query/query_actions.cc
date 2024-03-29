/*
* CPW Woodpecker Server
* Copyright (C) 2021 CPW Online support@cpwonline.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "query/query_actions.h"

using namespace CPW::Query;

QueryActions::QueryActions() :
    final_query_("")
    ,current_filters_(new Filters::Filters)
    ,incorporate_(new Filters::IncorporateFilters(current_filters_))
    ,result_json_(new JSON::Object)
    ,app_(Application::instance())
    ,identify_filter_(current_filters_)
{

}

QueryActions::~QueryActions()
{

}

void QueryActions::StartDatabase_()
{
    Data::MySQL::Connector::registerConnector();
    session_ = std::make_shared<Data::Session>("MySQL", "host=127.0.0.1;port=3306;db=cpw_woodpecker;user=root;password=mariadb_password;");
    query_ = std::make_shared<Data::Statement>(*session_);
}

void QueryActions::StopDatabase_()
{
    session_->close();
}

void QueryActions::IdentifyFilters_()
{
    try
    {
        Filters::FilterType type;
        auto pair = get_json_body()->getArray("pair-information");

        if(pair->size() < 1)
            throw std::runtime_error("pair-information size is < 1.");
        if(pair->get(1).isEmpty())
            throw std::runtime_error("pair object 1 is empty.");

        auto data_array = get_json_body()->getArray("pair-information")->getObject(1)->getArray("data");

        for (std::size_t a = 0; a < data_array->size(); a++)
        {
            // Get the temporal objects
                if(data_array->get(a).isEmpty())
                    throw std::runtime_error("Data array haves a empty object.");

                auto filter = data_array->getObject(a);
                if(filter->get("type").isEmpty())
                    throw std::runtime_error("An array object don't haves a type.");

            // Search if exists
                if(current_filters_->ExistsType_(filter->get("type").toString()))
                    type = current_filters_->get_filters_type_map().find(filter->get("type").toString())->second;
                else
                    continue;

                auto filter_var = data_array->get(a);

            // Manage the type
                auto functor = identify_filter_.get_filter_type_functors();
                functor[type](filter_var);
        }
    }
    catch(std::runtime_error& error)
    {
        app_.logger().error("- Error on query_actions.cc on IdentifyFilters_(): " + std::string(error.what()));
    }
    catch(JSON::JSONException& error)
    {
        app_.logger().error("- Error on query_actions.cc on IdentifyFilters_(): " + std::string(error.what()));
    }
}

void QueryActions::ComposeQuery_(TypeAction action_type, std::string table)
{
    std::string tmp_query;

    switch(action_type)
    {
        case TypeAction::kInsert:
        {
            tmp_query = ComposeInsertSentence_(table);
            break;
        }
        case TypeAction::kSelect:
        {
            tmp_query = ComposeSelectSentence_(table);
            break;
        }
        case TypeAction::kUpdate:
        {
            tmp_query = ComposeUpdateSentence_(table);
            break;
        }
        case TypeAction::kDelete:
        {
            tmp_query = ComposeDeleteSentence_(table);
            break;
        }
    }

    final_query_ = tmp_query;
    app_.logger().information("- Final query: " + tmp_query);
}

bool QueryActions::ExecuteQuery_(HTTPServerResponse& response)
{
    try
    {
        StartDatabase_();
        *query_ << final_query_, now;
        StopDatabase_();

        CreateJSONResult_();
    }
    catch(MySQL::MySQLException& error)
    {
        GenericResponse_(response, HTTPResponse::HTTP_BAD_REQUEST, std::string(error.message()));
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.message()));
        return false;
    }
    catch(JSON::JSONException& error)
    {
        GenericResponse_(response, HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, std::string(error.message()));
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.displayText()));
        return false;
    }
    catch(std::runtime_error& error)
    {
        GenericResponse_(response, HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, std::string(error.what()));
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.what()));
        return false;
    }
    catch(std::exception& error)
    {
        GenericResponse_(response, HTTPResponse::HTTP_INTERNAL_SERVER_ERROR, std::string(error.what()));
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.what()));
        return false;
    }

    return true;
}

bool QueryActions::ExecuteQuery_()
{
    try
    {
        StartDatabase_();
        *query_ << final_query_, now;
        StopDatabase_();

        CreateJSONResult_();
    }
    catch(MySQL::MySQLException& error)
    {
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.message()));
        return false;
    }
    catch(JSON::JSONException& error)
    {
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.displayText()));
        return false;
    }
    catch(std::runtime_error& error)
    {
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.what()));
        return false;
    }
    catch(std::exception& error)
    {
        app_.logger().error("- Error on query_actions.cc on ExecuteQuery_(): " + std::string(error.what()));
        return false;
    }

    return true;
}

void QueryActions::CreateJSONResult_()
{
    // Variables
        Data::RecordSet results(*query_);
        JSON::Array::Ptr results_array = new JSON::Array();
        JSON::Array::Ptr columns_array = new JSON::Array();

    // Save columns names
        for (std::size_t col = 0; col < results.columnCount(); ++col)
            columns_array->set(columns_array->size(), results.columnName(col));

    // Make JSON data
        for(auto& it : results)
        {
            JSON::Array::Ptr row_fields = new JSON::Array();

            for(size_t a = 0; a < it.fieldCount(); a++)
            {
                auto var = it.get(a);

                row_value_formatter_.reset(new Tools::RowValueFormatter(var));
                row_value_formatter_->Format_();
                switch(row_value_formatter_->get_row_value_type())
                {
                    case Tools::RowValueType::kEmpty:
                        row_fields->set(row_fields->size(), "");
                        break;
                    case Tools::RowValueType::kString:
                        row_fields->set(row_fields->size(), row_value_formatter_->get_value_string());
                        break;
                    case Tools::RowValueType::kInteger:
                        row_fields->set(row_fields->size(), row_value_formatter_->get_value_int());
                        break;
                    case Tools::RowValueType::kFloat:
                        row_fields->set(row_fields->size(), row_value_formatter_->get_value_float());
                        break;
                    default:
                        row_fields->set(row_fields->size(), "");
                        break;
                }
            }

            results_array->set(results_array->size(), row_fields);
        }

        result_json_->set("columns", columns_array);
        result_json_->set("results", results_array);
}

std::string QueryActions::ComposeInsertSentence_(std::string table)
{
    // Sentence type and Table
        std::vector<std::string> tmp_query = {"INSERT INTO " + table + " ("};

    // Fields
        incorporate_->IncorporateFields_(tmp_query);
        tmp_query.push_back(")");

    // Values
        tmp_query.push_back("VALUES");
        incorporate_->IncorporateValues_(tmp_query);

    tmp_query.push_back(";");

    std::string final_query = "";
    for(auto it : tmp_query)
        final_query += it + " ";

    return final_query;
}

std::string QueryActions::ComposeSelectSentence_(std::string table)
{
    // Sentence type and fields
        std::vector<std::string> tmp_query = {"SELECT"};
        incorporate_->IncorporateFields_(tmp_query);

    // Table
        tmp_query.push_back("FROM " + table);

    // Joins
        incorporate_->IncorporateJoins_(tmp_query);

    // Conditions
        incorporate_->IncorporateIqual_(tmp_query);
        incorporate_->IncorporateNotIqual_(tmp_query);
        incorporate_->IncorporateGreatherThan_(tmp_query);
        incorporate_->IncorporateSmallerThan_(tmp_query);
        incorporate_->IncorporateBetween_(tmp_query);
        incorporate_->IncorporateIn_(tmp_query);
        incorporate_->IncorporateNotIn_(tmp_query);
        incorporate_->IncorporateLike_(tmp_query);

    // Sort conditions
        incorporate_->IncorporateSort_(tmp_query);

    // Page and Limit condition
        incorporate_->IncorporatePageLimit_(tmp_query, true);

    return MakeFinalQuery_(tmp_query);
}

std::string QueryActions::ComposeUpdateSentence_(std::string table)
{
    // Sentence type and table
        std::vector<std::string> tmp_query = {"UPDATE"};
        tmp_query.push_back(table);

    // Joins
        incorporate_->IncorporateJoins_(tmp_query);

    // Set
        tmp_query.push_back("SET");
        incorporate_->IncorporateSet_(tmp_query);

    // Conditions
        incorporate_->IncorporateIqual_(tmp_query);
        incorporate_->IncorporateNotIqual_(tmp_query);
        incorporate_->IncorporateGreatherThan_(tmp_query);
        incorporate_->IncorporateSmallerThan_(tmp_query);
        incorporate_->IncorporateBetween_(tmp_query);
        incorporate_->IncorporateIn_(tmp_query);
        incorporate_->IncorporateNotIn_(tmp_query);
        incorporate_->IncorporateLike_(tmp_query);

    // Sort conditions
        incorporate_->IncorporateSort_(tmp_query);

    // Page and Limit condition
        incorporate_->IncorporatePageLimit_(tmp_query, false);

    return MakeFinalQuery_(tmp_query);
}

std::string QueryActions::ComposeDeleteSentence_(std::string table)
{
    // Sentence type and Table
        std::vector<std::string> tmp_query = {"DELETE"};
        incorporate_->IncorporateFields_(tmp_query, false);

        tmp_query.push_back("FROM " + table);

    // Joins
        incorporate_->IncorporateJoins_(tmp_query);

    // Conditions
        incorporate_->IncorporateIqual_(tmp_query);
        incorporate_->IncorporateNotIqual_(tmp_query);
        incorporate_->IncorporateGreatherThan_(tmp_query);
        incorporate_->IncorporateSmallerThan_(tmp_query);
        incorporate_->IncorporateBetween_(tmp_query);
        incorporate_->IncorporateIn_(tmp_query);
        incorporate_->IncorporateNotIn_(tmp_query);
        incorporate_->IncorporateLike_(tmp_query);

    // Sort conditions
        incorporate_->IncorporateSort_(tmp_query);

    // Page and Limit condition
        incorporate_->IncorporatePageLimit_(tmp_query, false);

    return MakeFinalQuery_(tmp_query);
}

std::string QueryActions::MakeFinalQuery_(std::vector<std::string>& tmp_query)
{
    tmp_query.push_back(";");
    std::string final_query = "";

    for(auto it : tmp_query)
        final_query += it + " ";

    return final_query;
}
