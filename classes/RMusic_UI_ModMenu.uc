class RMusic_UI_ModMenu extends UMenuModMenuItem;

function Setup()
{
	MenuCaption = "RMusicPlayer Volume Control";
	MenuHelp = "Cliclk to change music volume";
}

function Execute()
{

	MenuItem.Owner.Root.CreateWindow(Class<UWindowFramedWindow>(DynamicLoadObject("RMusicPlayer.RMusic_UI_FramedWindow", class'Class')),20,20,170,200);
}

