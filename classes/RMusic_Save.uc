/**
 * Copyright 2008 Raven
 */
// Stores last used RMusic_Controller when game is saved.
class RMusic_Save extends RMusic_Component
				NoUserCreate;

var RMusic_Controller SavedController;

defaultproperties
{
	bStatic=false
	bStasis=false
	bNoDelete=false
}