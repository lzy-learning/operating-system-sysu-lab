#include<iostream>

extern "C" void function_from_asm();
int id = 21307403;
int main(void){
	std::cout<<"call function from assembly." <<std::endl;
	function_from_asm();
	std::cout<<"Done by 21307403 LZY"<<std::endl;
	return 0;
}
