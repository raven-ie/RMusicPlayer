@cd..
@del system\RMusicPlayer.u
@system\ucc make ini=../RMusicPlayer/make.ini
@copy system\RMusicPlayer.u rmusicplayer\release\_ready\RMusicPlayer.u
@copy system\RMusicPlayer.dll rmusicplayer\release\_ready\RMusicPlayer.dll
@pause
