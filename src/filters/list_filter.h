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

#ifndef CPW_FILTERS_LIST_FILTER_H
#define CPW_FILTERS_LIST_FILTER_H


#include "filters/filter.h"


namespace CPW
{
    namespace Filters
    {
        class ListFilterElement;
        class ListFilter;
    }
}


class CPW::Filters::ListFilterElement
{
    public:
        enum class Type
        {
            kIn
            ,kNotIn
        };

        ListFilterElement(std::string col, std::list<Extras::ValuesProperties> values, std::string type);

        std::string get_col() const { return col_; }
        std::list<Extras::ValuesProperties>& get_values()
        {
            auto& var = values_;
            return var;
        }
        Type get_type() const { return type_; }
        std::map<std::string, Type>& get_types()
        {
            auto& var = types_;
            return var;
        }

        void set_col(std::string col) { col_ = col; }
        void set_type(Type type) { type_ = type; }

    protected:
        void AddTypes_();

    private:
        std::string col_;
        std::list<Extras::ValuesProperties> values_;
        Type type_;
        std::map<std::string, Type> types_;
};

class CPW::Filters::ListFilter : Filters::Filter
{
    public:
        ListFilter();
        virtual ~ListFilter();

        std::list<Filters::ListFilterElement>& get_filter_elements()
        {
            auto& var = filter_elements_;
            return var;
        }

        virtual void Identify_(Dynamic::Var& filter) override;
        virtual void Incorporate_(VectorString& tmp_query) override;

    private:
        std::list<Filters::ListFilterElement> filter_elements_;
};


#endif // CPW_FILTERS_LIST_FILTER_H
