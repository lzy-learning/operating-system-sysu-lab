#include<iostream>

extern "C" void function_from_cpp(char* name){
	std::cout<<"this is function from cpp called by "<<name<<std::endl;
}
