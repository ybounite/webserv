#include "Socket.hpp"

int main()
{
    try
    {
        Socket sct;
        sct.run();

    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}