/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2021  <copyright holder> <email>
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

#include "tools/manage_json.h"

using namespace CPW::Tools;

ManageJSON::ManageJSON()
{

}

ManageJSON::~ManageJSON()
{

}

std::string ManageJSON::ReadBody_(std::istream& stream)
{
	std::string json_body;
	StreamCopier::copyToString(stream, json_body);

	if(json_body.empty())
		return "";

	return json_body;
}

bool ManageJSON::Parse_(std::string string_to_parse)
{
	if(string_to_parse.empty())
		return false;

	JSON::Parser parser;

	Dynamic::Var var_tmp = parser.parse(string_to_parse);

	if(var_tmp.isArray())
		return false;

	dynamic_json_body_ = *var_tmp.extract<JSON::Object::Ptr>();

	return VerifyJSON_();
}

bool ManageJSON::VerifyJSON_()
{
	if
	(
		dynamic_json_body_["pair-information"].isEmpty()
		|| dynamic_json_body_["pair-information"][0]["auth"].isEmpty()
		|| dynamic_json_body_["pair-information"][1]["data"].isEmpty()
		|| dynamic_json_body_["pair-information"][1]["data"].size() < 1
	)
		return false;
	else{
		return true;}
}
