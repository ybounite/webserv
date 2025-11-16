/* ************************************************************************** */
/*                    file: Request.hpp                                       */
/*                    by: ybounite <ybounite@student.1337.ma>                 */
/*                                                                            */
/*                    Created: 2025/11/16 09:36 by ybounite                   */
/*                    Updated: 2025/11/16 09:36 by ybounite                   */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <string>

/* ************************************************************************** */
/*                          Request Class                                     */
/* ************************************************************************** */

class Request {
public:

	Request( void );
	Request( const  Request &Other );
	Request &operator=( const  Request &Other );
	~Request( void );
private:
};
