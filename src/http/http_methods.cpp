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

#include "http/http_methods.h"

using namespace CPW::HTTP;

HTTPMethods::HTTPMethods()
{

}

HTTPMethods::~HTTPMethods()
{

}

void HTTPMethods::HandleGETMethod_()
{
    if(!QueryProcess_(Query::TypeAction::kSelect))
    {
        responses_.GenericResponse_(*dynamic_elements_->get_response(), HTTPResponse::HTTP_BAD_REQUEST, "Error when performing database queries.");
        return;
    }

    responses_.CompoundResponse_
    (
        *dynamic_elements_->get_response()
        ,HTTPResponse::HTTP_OK
        ,"Ok."
        ,dynamic_elements_->get_query_actions()->get_result_json()
        ,dynamic_elements_->get_query_actions()->get_affected_rows_()
    );
}

void HTTPMethods::HandlePOSTMethod_()
{
    if(!QueryProcess_(Query::TypeAction::kInsert))
        return;

    responses_.GenericResponse_(*dynamic_elements_->get_response(), HTTPResponse::HTTP_OK, "Ok.");
}

void HTTPMethods::HandlePUTMethod_()
{
    if(!QueryProcess_(Query::TypeAction::kUpdate))
        return;

    responses_.GenericResponse_(*dynamic_elements_->get_response(), HTTPResponse::HTTP_OK, "Ok.");
}

void HTTPMethods::HandleDELMethod_()
{
    if(!QueryProcess_(Query::TypeAction::kDelete))
        return;

    responses_.GenericResponse_(*dynamic_elements_->get_response(), HTTPResponse::HTTP_OK, "Ok.");
}

bool HTTPMethods::QueryProcess_(Query::TypeAction action)
{
    auto query_manager = dynamic_elements_->get_query_actions();

    query_manager->IdentifyFilters_();
    if(!query_manager->ComposeQuery_(action, dynamic_elements_->get_requested_route()->get_target()))
        return false;
    if(!query_manager->ExecuteQuery_(*dynamic_elements_->get_response()))
        return false;
    if(!query_manager->CreateJSONResult_())
        return false;

    return true;
}
