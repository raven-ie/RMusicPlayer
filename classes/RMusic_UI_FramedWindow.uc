class RMusic_UI_FramedWindow extends UWindowFramedWindow;

function BeginPlay()
{
	Super.BeginPlay();
	WindowTitle = "RMusicPlayer :: Volume Control";
	ClientClass = class'RMusicPlayer.RMusic_UI_ClientWindow';
	bSizable = false;
}

function Created()
{
	Super.Created();
	SetSize(170, 64);
	WinLeft = (Root.WinWidth - 170) / 2;
	WinTop = (Root.WinHeight - 64) / 2;
}
