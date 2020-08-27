#pragma once

#include <exception>
#include <string>

class myException :public std::exception {

public:

	myException(int line, const char* file) noexcept;
	
	//print out exception line number & thrown on file
	const char* what() const noexcept override;

	//giving exception a type name
	virtual const char* GetType() const noexcept;
	
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;

	//get the origin string(long string including type,line and file
	std::string GetOriginString() const noexcept;

private:
	int line;		//line number
	std::string file;		//file it was thrown on

protected:
	mutable std::string whatBuffer;

};