
#include<exception>
#include<string>




namespace vizpet{
    class ModelInitError:public std::exception{
            std:: string error_msg;
        public:
            
            ModelInitError(std::string error_msg){
                this->error_msg=error_msg;
            }

            ~ModelInitError() throw(){}

            const char * what () const throw (){
    	        return error_msg.c_str();
            }
    };

}