#include "GuiNetPlaySettings.h"
#include "SystemConf.h"
#include "ThreadedHasher.h"
#include "components/SwitchComponent.h"
#include "GuiHashStart.h"

GuiNetPlaySettings::GuiNetPlaySettings(Window* window) : GuiSettings(window, _("NETPLAY SETTINGS").c_str())
{
	std::string port = SystemConf::getInstance()->get("global.netplay.port");
	if (port.empty())
		SystemConf::getInstance()->set("global.netplay.port", "55435");

	addGroup(_("SETTINGS"));

	auto enableNetplay = std::make_shared<SwitchComponent>(mWindow);
	enableNetplay->setState(SystemConf::getInstance()->getBool("global.netplay"));
	addWithLabel(_("ENABLE NETPLAY"), enableNetplay);
	addInputTextRow(_("NICKNAME"), "global.netplay.nickname", false);

	addGroup(_("OPTIONS"));

	addInputTextRow(_("PORT"), "global.netplay.port", false);
	addOptionList(_("USE RELAY SERVER"), { { _("NONE"), "" },{ _("NEW YORK") , "nyc" },{ _("MADRID") , "madrid" },{ _("MONTREAL") , "montreal" },{ _("SAO PAULO") , "saopaulo" },{ _("CUSTOM") , "custom" } }, "global.netplay.relay", false);
	addInputTextRow(_("CUSTOM RELAY SERVER"), "global.netplay.customserver", false);

	auto public_announce = std::make_shared<SwitchComponent>(mWindow);
	public_announce->setState(SystemConf::getInstance()->getBool("global.netplay_public_announce"));
	addWithLabel(_("PUBLICLY ANNOUNCE GAME"), public_announce);
	addSaveFunc([public_announce] { SystemConf::getInstance()->setBool("global.netplay_public_announce", public_announce->getState()); });

	addInputTextRow(_("PLAYER PASSWORD"), "global.netplay.password", false);
	addInputTextRow(_("VIEWER PASSWORD"), "global.netplay.spectatepassword", false);

	addSwitch(_("SHOW NETPLAY OPTIONS WHEN LAUNCHING GAME"), _("Allows choice of online, offline, or joining another game."), "NetPlayShowOptionsWhenLaunchingGames", true, nullptr);
	addSwitch(_("SHOW RELAY SERVER GAMES ONLY"), _("Relay server games have a higher chance of successful entry."), "NetPlayShowOnlyRelayServerGames", true, nullptr);
	addSwitch(_("SHOW UNAVAILABLE GAMES"), _("Show rooms for games not present on this machine."), "NetPlayShowMissingGames", true, nullptr);

	addGroup(_("GAME INDEXES"));

	addSwitch(_("INDEX NEW GAMES AT STARTUP"), "NetPlayCheckIndexesAtStart", true);
	addEntry(_("INDEX GAMES"), true, [this]
	{
		if (ThreadedHasher::checkCloseIfRunning(mWindow))
			mWindow->pushGui(new GuiHashStart(mWindow, ThreadedHasher::HASH_NETPLAY_CRC));
	});

	Window* wnd = mWindow;
	addSaveFunc([wnd, enableNetplay]
	{
		if (SystemConf::getInstance()->setBool("global.netplay", enableNetplay->getState()))
		{
			if (!ThreadedHasher::isRunning() && enableNetplay->getState())
			{
				ThreadedHasher::start(wnd, ThreadedHasher::HASH_NETPLAY_CRC, false, true);
			}
		}
	});
}
