#include "../includes/Webserv.hpp"

std::vector<std::string> Tokenizer::tokenize(const std::string &content)
{
    std::vector<std::string> vector;
    unsigned long i = 0;
    std::string str;
    bool flag;
    bool flag_str;

    while (i < content.length())
    {
        flag = false;
        flag_str = false;

        // Skip all whitespace first
        while (i < content.length() && 
              (content[i] == '\n' || content[i] == 32 || (content[i] <= 9 && content[i] >= 1)))
        {
            i++;
        }

        if (i >= content.length())
            break;

        // Single-character tokens
        if (content[i] == '{' || content[i] == '}' || content[i] == ';')
        {
            vector.push_back(std::string(1, content[i]));
            i++;
            continue;
        }

        // Build token
        while (i < content.length())
        {
            if (content[i] == '\n')
                break;

            if (content[i] == '{' || content[i] == '}' || content[i] == ';')
            {
                flag = true;
                break;
            }

            if (content[i] == 32 || (content[i] <= 9 && content[i] >= 1))
                break;

            str += content[i];
            flag_str = true;
            i++;
        }

        if (flag_str)
            vector.push_back(str);

        str.clear();
    
        if (!flag && i < content.length() && content[i] != '{' && content[i] != '}' && content[i] != ';')
            i++;
    }

    return vector;
}