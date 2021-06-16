#ifndef LIB_GREET_H_
#define LIB_GREET_H_

#include <string>

#define popen() { LOG(ERROR) << "popen should never be called on web."; return NULL; }


namespace HelloWorld {

    class Greet {
        public:
        
        /*
        * Greets the name 
        */
        static std::string SayHello(const std::string &name);
    };

} // namespace HelloWorld

#endif
