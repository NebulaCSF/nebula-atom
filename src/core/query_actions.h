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

#ifndef CPW_QUERYACTIONS_H
#define CPW_QUERYACTIONS_H


#include <istream>
#include <string>
#include <map>
#include <stdexcept>

#include "Poco/Util/Application.h"
#include <Poco/Net/HTTPServerRequest.h>
#include "Poco/Data/Session.h"
#include "Poco/Data/MySQL/Connector.h"
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/Statement.h>
#include <Poco/URI.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/JSONException.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/Data/RecordSet.h>

#include "tools/manage_json.h"
#include "tools/filters.h"


namespace CPW
{
	namespace Core
	{
		enum class TypeAction;
		enum class TypeQuery;
		class QueryActions;
	}
}

using namespace Poco;
using namespace Poco::Util;
using namespace Poco::Net;
using namespace Poco::Data;
using namespace Poco::Data::Keywords;


enum class CPW::Core::TypeAction
{
	kInsert
	,kSelect
	,kUpdate
	,kDelete
};

enum class CPW::Core::TypeQuery
{
	kFields
	,kPage
	,kLimit
	,kSort
	,kIqual
	,kNotIqual
	,kGreatherThan
	,kSmallerThan
	,kBetween
	,kIn
	,kNotIn
	,kValues
	,kSet
};


class CPW::Core::QueryActions : public CPW::Tools::ManageJSON
{
	public:
		QueryActions();
		~QueryActions();

		std::string get_final_query() const {return final_query_;}
		CPW::Tools::Filters& get_current_filters_()
		{
			auto& var = current_filters_;
			return var;
		}
		Data::Session* get_session() const {return session_.get();}
		Data::Statement* get_query() const {return query_.get();}
		std::map<std::string, std::string>* get_table_rows() const {return table_rows_;}
		JSON::Array::Ptr get_result_json() const {return result_json_;}

		void StartDatabase_();
		void StopDatabase_();
		void IdentifyFilters_();
		void ComposeQuery_(TypeAction action_type, std::string table);
		void ExecuteQuery_();

	protected:
		void CreateJSONResult_();
		std::string ComposeInsertSentence_(std::string table);
		std::string ComposeSelectSentence_(std::string table);
		std::string ComposeUpdateSentence_(std::string table);
		std::string ComposeDeleteSentence_(std::string table);
		std::string MakeFinalQuery_(std::vector<std::string>& tmp_query);

		void IncorporeWhere_(std::vector<std::string>& tmp_query);
		void IncorporeAND_(std::vector<std::string>& tmp_query);
		void IncorporeFields_(std::vector<std::string>& tmp_query);
		void IncorporePageLimit_(std::vector<std::string>& tmp_query);
		void IncorporeSort_(std::vector<std::string>& tmp_query);
		void IncorporeIqual_(std::vector<std::string>& tmp_query);
		void IncorporeNotIqual_(std::vector<std::string>& tmp_query);
		void IncorporeGreatherThan_(std::vector<std::string>& tmp_query);
		void IncorporeSmallerThan_(std::vector<std::string>& tmp_query);
		void IncorporeBetween_(std::vector<std::string>& tmp_query);
		void IncorporeIn_(std::vector<std::string>& tmp_query);
		void IncorporeNotIn_(std::vector<std::string>& tmp_query);
		void IncorporeValues_(std::vector<std::string>& tmp_query);
		void IncorporeSet_(std::vector<std::string>& tmp_query);

	private:
		void FillTypeActionsText_();
		bool ExistsType_(std::string type);

		std::string final_query_;
		CPW::Tools::Filters current_filters_;
		std::unique_ptr<Data::Session> session_;
		std::unique_ptr<Data::Statement> query_;
		std::map<std::string, std::string>* table_rows_;
		JSON::Array::Ptr result_json_;
		std::map<std::string, TypeQuery> type_actions_map_;
		Application& app_;
};


#endif // CPW_QUERYACTIONS_H
