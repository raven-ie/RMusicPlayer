class RMusic_UI_ClientWindow extends UWindowDialogClientWindow;

var UWindowHSliderControl MusicSlider;
var localized string MusicText;
var localized string MusicHelp;
var UWindowSmallCloseButton CloseButton;

function Created()
{
	Super.Created();
	SetSize(170, 128);
	WinWidth=170;
	WinHeight=64;

	MusicSlider = UWindowHSliderControl(CreateControl(class'UWindowHSliderControl', 5, 10, 150, 1));
	MusicSlider.SetRange(0, 100, 1);
	MusicSlider.SetValue(class'RMusic_Player'.default.RMusic_Volume);
	MusicSlider.SetText(MusicText@"("$int(MusicSlider.Value)$")");
	MusicSlider.SetHelpText(MusicHelp);
	MusicSlider.SetFont(F_Normal);
	CloseButton = UWindowSmallCloseButton(CreateWindow(class'UWindowSmallCloseButton', 61, 24, 48, 16));
}

function Notify(UWindowDialogControl C, byte E)
{
	switch(E)
	{
		case DE_Change:
			switch(C)
			{
				case MusicSlider:
					class'RMusic_Player'.default.RMusic_Volume=MusicSlider.Value;
					class'RMusic_Player'.static.StaticSaveConfig();
					EVENT_VolumeChanged();
				break;
			}
		break;
	}
}

simulated function EVENT_VolumeChanged()
{
	local RMusic_Player RMusic_Player;

	MusicSlider.SetText(MusicText@"("$int(MusicSlider.Value)$")");

	ForEach GetPlayerOwner().GetEntryLevel().AllActors(class'RMusicPlayer.RMusic_Player', RMusic_Player)
	{
		break;
	}

	if(RMusic_Player != none)
	{
		RMusic_Player.RMusic_SetVolume(MusicSlider.Value);
	}
}

defaultproperties
{
	MusicText="Volume"
	MusicHelp="Will change music volume"
}
