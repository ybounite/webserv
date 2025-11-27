#include "Server.hpp"

int main()
{
    try
    {
        Server sct;
        sct.run();

    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}