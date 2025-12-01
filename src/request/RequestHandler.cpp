/* ************************************************************************** */
/*                    file: RequestHandler.cpp                                */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/29 16:09 by ybounite                   */
/*                    Updated: 2025/11/29 16:09 by ybounite                   */
/* ************************************************************************** */

# include "RequestHandler.hpp"

RequestHandler::RequestHandler( void ) {

}
RequestHandler::RequestHandler( const Request &Other ) : Request(Other) {}

RequestHandler::RequestHandler( const  RequestHandler &Other ){
    *this = Other;
}

RequestHandler::~RequestHandler( void ) {

}

void    RequestHandler::handle(const Config &config) {

    (void)config;
    std::cout << "Method is : " << this->getMethod() << std::endl;
}

// Response    RequestHandler::handleGET(const Request req, const ServerConfig &config)
// {
//     (void)req;(void)config;
//     // std::cout << req.getUri() << std::endl;
//     Response res;
//     // // res.uri = "asdg";
//     // // ---------------------------------------------------------
//     // // 1. Build the real file path from URI
//     // // ---------------------------------------------------------
//     // std::string path = config.root + req.getUri();
    
//     // // If URI is directory → add index file
//     // if (req.uri.back() == '/')
//     //     path += "index.html";

//     // // ---------------------------------------------------------
//     // // 2. Check if file exists
//     // // ---------------------------------------------------------
//     // struct stat st;
//     // if (stat(path.c_str(), &st) == -1) {
//     //     res.StatusCode = 404;
//     //     res.Body = "<h1>404 Not Found</h1>";
//     //     res.Headers["Content-Type"] = "text/html";
//     //     res.Headers["Content-Length"] = std::to_string(res.Body.size());
//     //     return res;
//     // }

//     // // ---------------------------------------------------------
//     // // 3. Read file content
//     // // ---------------------------------------------------------
//     // std::ifstream file(path.c_str(), std::ios::binary);
//     // if (!file) {
//     //     res.StatusCode = 500;
//     //     res.Body = "<h1>500 Internal Server Error</h1>";
//     //     res.Headers["Content-Type"] = "text/html";
//     //     res.Headers["Content-Length"] = std::to_string(res.Body.size());
//     //     return res;
//     // }

//     // std::ostringstream buffer;
//     // buffer << file.rdbuf();
//     // res.Body = buffer.str();

//     // // ---------------------------------------------------------
//     // // 4. Build successful response
//     // // ---------------------------------------------------------
//     // res.StatusCode = 200;
//     // res.Headers["Content-Type"] = "text/html";     // you can add MIME detection later
//     // res.Headers["Content-Length"] = std::to_string(res.Body.size());

//     return res;
// }