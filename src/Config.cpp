#include "../includes/Webserv.hpp"

void parse_config(Config &config)
{
    unsigned long i = 0;
    unsigned long j = 0;
    unsigned long index_of_methods = 0;

    while (config.servers.size() > i )
    {
        j = 0;
        while (config.servers[i].locations.size() > j)
        {
            index_of_methods = 0;
            while (config.servers[i].locations[j].methods.size() > index_of_methods)
            {
                std::string methods = config.servers[i].locations[j].methods[index_of_methods] ;
                if (methods != "GET" && methods != "POST" && methods != "DELETE")
                    throw std::runtime_error("Unexpected methods our webserv take just ` GET POST DELETE `: " + methods);
                index_of_methods++;
            }
            
            j++;
        }
        
        i++;
    }
    
}
