/*
* Nebula Atom

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

#ifndef ATOMFILTERS_FILTER_H
#define ATOMFILTERS_FILTER_H


#include <string>
#include <map>
#include <vector>
#include <list>
#include <forward_list>
#include <functional>
#include <stdexcept>
#include <algorithm>

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/JSONException.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Dynamic/Struct.h>

#include "tools/row_value_formatter.h"
#include "extras/values_properties.h"
#include "tools/manage_json.h"


namespace Atom
{
    namespace Filters
    {
        enum class FilterType;

        template <class T>
        class FilterElements;

        class Filter;
    }
}

using namespace Poco;

using namespace AtomExtras;


enum class AtomFilters::FilterType
{
    kUnknown
    ,kFields
    ,kSort
    ,kGeneral
    ,kIqual
    ,kRange
    ,kList
    ,kLike
    ,kJoin
    ,kGroup
    ,kValues
    ,kSet
};


class AtomFilters::Filter
{
    public:
        using RowValueFormatterList = std::list<Tools::RowValueFormatter>;
        using VectorString = std::vector<std::string>;

        Filter();
        virtual ~Filter();

        Filters::FilterType& get_current_filter_type()
        {
            auto& var = current_filter_type_;
            return var;
        }
        Tools::ManageJSON& get_manage_json()
        {
            auto& var = manage_json_;
            return var;
        }

        virtual void Identify_(Dynamic::Var& filter) = 0;
        virtual void Incorporate_(VectorString& tmp_query, RowValueFormatterList& query_parameters) = 0;

    protected:
        bool FindWHERE_(VectorString& tmp_query);
        bool FindAND_(VectorString& tmp_query);
        Extras::ValuesProperties GetValueProperties_(Dynamic::Var& var);

    private:
        Filters::FilterType current_filter_type_;
        Tools::ManageJSON manage_json_;
};


#endif // ATOMFILTERS_FILTER_H
