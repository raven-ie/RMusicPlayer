#ifndef _INC_RSTRING
#define _INC_RSTRING

FString AddSlashes(FString Text);
FString ConnectAddSlashes(FString Text, FString File);
FString RemoveSpaces(FString Text);
UBOOL FStringDivide(FString Src, FString Divider, FString& LeftPart, FString& RightPart);
FString FStrReplace(FString Text, FString Replace, FString With);

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Part created by ENIGMA - Big THX men :)
*/
// new function
// handle the case where TCHAR is char
std::string toAnsiString(char const * const string);
// new function
// handle the case where TCHAR is wchar_t
std::string toAnsiString(wchar_t const * const string);
std::string toAnsiString(FString * string);

#endif