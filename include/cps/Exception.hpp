#ifndef CPS_EXCEPTION_HPP
#define CPS_EXCEPTION_HPP

#include <boost/exception/all.hpp>
#include <boost/shared_ptr.hpp>

#include "Utils.hpp"

namespace CPS
{

class Response;

class Exception: public boost::exception, public std::exception
{
public:
    /**
     * CPS Exception class
     * @param longMessage a descriptive message about error [default = empty]
     * @param errorCode an error code for message [default = 0]
     */
    Exception(std::string longMessage = "", unsigned int errorCode = 0):
    	longMessage(longMessage), errorCode(errorCode) {
        this->whatString = ("[" + CPS::Utils::toString(errorCode) + "]: " + longMessage);
    }

    /**
     * CPS Exception class
     * @param longMessage a descriptive message about error
     * @param errorCode an error code for message
     * @param resp Response object
     */
    Exception(std::string longMessage, unsigned int errorCode, Response *resp):
    	longMessage(longMessage), errorCode(errorCode), response(resp) {
        this->whatString = ("[" + CPS::Utils::toString(errorCode) + "]: " + longMessage);
    }

    virtual ~Exception() throw () {
    }

    /**
     * Returns string of error message in format:
     * [Error Code]: Message
     */
    virtual const char* what() const throw() {
        return whatString.c_str();
    }

    boost::shared_ptr<Response> getResponse() {
    	return response;
    }

    std::string longMessage;
    unsigned int errorCode;
private:
    std::string whatString;
    boost::shared_ptr<Response> response;
};
}

#endif //#ifndef CPS_EXCEPTION_HPP 
