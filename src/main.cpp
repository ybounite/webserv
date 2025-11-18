#include "../includes/Webserv.hpp"

// int main(int argc, char const *argv[])
// {
//     if (argc == 1 || argc == 2)
//     {

//     }
//     else
//     {

//     }
//     return 0;
// }


int main()
{
    Msg::info("Reading configuration file...");
    Msg::success("Server started on port 8080");
    Msg::warning("Missing index directive");
    Msg::error("Failed to open file!");
    Msg::debug("Token count = 153");
    Msg::print("Hello sodahani!", LIGHTMAGENTA);
    Msg::print("Danger!", BLINK);
    Msg::print("Sky Blue!", LIGHT_BLUE);
}