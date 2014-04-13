/**
 * Copyright 2008 Raven
 */
#include "RMusicPlayerPrivate.h"

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Add slashes
*/
FString AddSlashes(FString Text)
{
	guard(AddSlashes);
	int i;
	FString InputT;
	FString Replace = FString(TEXT("/"));
	FString With = FString(TEXT("//"));

	InputT = Text;
	Text = TEXT("");
	i = InputT.InStr(Replace);
	while (i != -1)
	{
		Text = Text + InputT.Left(i) + With;
		InputT = InputT.Mid(i + Replace.Len());
		i = InputT.InStr(Replace);
	}
	Text = Text + InputT;
	return Text;
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Add slashes and FString File at the end
*/
FString ConnectAddSlashes(FString Text, FString File)
{
	guard(ConnectAddSlashes);
	int i;
	FString InputT;
	FString Replace = FString(TEXT("/"));
	FString With = FString(TEXT("//"));

	InputT = Text;
	Text = TEXT("");
	i = InputT.InStr(Replace);
	while (i != -1)
	{
		Text = Text + InputT.Left(i) + With;
		InputT = InputT.Mid(i + Replace.Len());
		i = InputT.InStr(Replace);
	}
	Text = Text + InputT + File;
	return Text;
	unguard;
}
/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Removes spaces
*/
FString RemoveSpaces(FString Text)
{
	guard(RemoveSpaces);
	int i;
	FString InputT;
	FString Replace = FString(TEXT(" "));
	FString With = FString(TEXT(""));

	InputT = Text;
	Text = TEXT("");
	i = InputT.InStr(Replace);
	while (i != -1)
	{
		Text = Text + InputT.Left(i) + With;
		InputT = InputT.Mid(i + Replace.Len());
		i = InputT.InStr(Replace);
	}
	Text = Text + InputT;
	return Text;
	unguard;
}
UBOOL FStringDivide(FString Src, FString Divider, FString& LeftPart, FString& RightPart)
{
	int div_pos = Src.InStr(Divider);

	if (div_pos != -1)
	{
		LeftPart = Src.Left(div_pos);
		RightPart = Src.Mid(LeftPart.Len() + Divider.Len(), Src.Len() - LeftPart.Len() - Divider.Len());
		return 1;
	}
	else
		return 0;
}

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* replaces 'replace' with 'with' in text
*/
FString FStrReplace(FString Text, FString Replace, FString With)
{
	int i;
	FString InputT;

	InputT = Text;
	Text = TEXT("");
	i = InputT.InStr(Replace);
	while (i != -1)
	{
		Text = Text + InputT.Left(i) + With;
		InputT = InputT.Mid(i + Replace.Len());
		i = InputT.InStr(Replace);
	}
	Text = Text + InputT;
	return Text;
}

// new function
// handle the case where TCHAR is wchar_t
std::string toAnsiString(wchar_t const * const string)
{
	// create a vector big enough to hold the converted string
	std::vector< char > convertedCharacters(wcstombs(0, string, 0));
	// convert the characters
	wcstombs(&convertedCharacters[0], string, convertedCharacters.size());
	// convert to a string and return
	return std::string(convertedCharacters.begin(), convertedCharacters.end());
}

std::string toAnsiString(FString * string)
{
	return toAnsiString(**string);
}