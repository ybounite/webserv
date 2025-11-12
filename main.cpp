#include "Socket.hpp"

int main()
{
    try
    {
        Socket sct;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }

}