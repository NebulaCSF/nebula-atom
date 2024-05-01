
#include "core/nebula_atom.h"
#include "functions/action.h"
#include "handlers/custom_handler.h"
#include "http/body.h"
#include "http/request.h"
#include "tools/output_logger.h"
#include <Poco/JSON/Object.h>

using namespace Atom;

void ViewParameters(Handlers::CustomHandler& self)
{
    switch(self.get_body_type())
    {
        case HTTP::Body::Type::kFormMultipart:
        {
            for(auto& file : self.get_files_parameters().get_files())
                Tools::OutputLogger::Log_("FormMultipart parameter: " + file.get_name() + ", " + file.get_filename());
            for(auto& value : *self.get_form())
                Tools::OutputLogger::Log_("FormMultipart parameter: " + value.first + ", " + value.second);
            break;
        }
        case HTTP::Body::Type::kJSON:
        {
            self.get_json_array()->stringify(std::cout);
            self.get_json_object()->stringify(std::cout);
            break;
        }
        case HTTP::Body::Type::kFormURLEncoded:
        case HTTP::Body::Type::kURI:
        {
            for(auto param : self.get_query_parameters())
                Tools::OutputLogger::Log_("URI parameter: " + param.first + ", " + param.second);
            self.get_json_array()->stringify(std::cout);
            break;
        }
    }
}

int main(int argc, char** argv)
{
    Core::NebulaAtom app;

    Query::DatabaseManager::StartMySQL_();
    Tools::SettingsManager::ReadBasicProperties_();

    app.CustomHandlerCreator_([&](Core::HTTPRequestInfo&)
    {
        return new Handlers::CustomHandler([&](Handlers::CustomHandler& self)
        {
            // Process the request body
            self.ManageRequestBody_();

            // View parameters
            ViewParameters(self);

            // Create query
            Functions::Action a1("a1");
            a1.set_sql_code("SELECT * FROM products");

            a1.ComposeQuery_();
            a1.ExecuteQuery_();
            a1.MakeResults_();

            auto result = a1.CreateJSONResult_();
            result->set("status", a1.get_status());
            result->set("message", a1.get_message());

            self.CompoundResponse_(HTTP::Status::kHTTP_OK, result);
        });
    });

    // Run
        auto code = app.Init_(argc, argv);

    // End
        Query::DatabaseManager::StopMySQL_();
        return code;
}
