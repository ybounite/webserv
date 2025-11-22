#include "../includes/Webserv.hpp"

void printVector(const std::vector<std::string>& vec, const std::string& name)
{
    std::cout << name << ": ";
    std::cout << std::endl;
    for (size_t i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] ;
        std::cout << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    std::vector <std::string> vector;
    if (argc == 1 || argc == 2)
    {
        try
        {
            std::string		config_path;
            Tokenizer token;
            // Parser Parser;
            config_path = (argc == 1 ? "configs/default.conf" : argv[1]);
            vector = token.tokenize(ConfigFileReader::read(config_path));
            // Parser.parse(vector);

            printVector(vector, "vector");
            // std::cout << ConfigFileReader::read(config_path) << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    else
    {
        Msg::error("Bad arguments, should be ./webserv [configuration file], ");
        return 1;
    }
    return 0;
}


// int main()
// {
//     Msg::info("Reading configuration file...");
//     Msg::success("Server started on port 8080");
//     Msg::warning("Missing index directive");
//     Msg::error("Failed to open file!");
//     Msg::debug("Token count = 153");
//     Msg::print("Hello sodahani!", LIGHTMAGENTA);
//     Msg::print("Danger!", BLINK);
//     Msg::print("Sky Blue!", LIGHT_BLUE);
// }