# Microsoft Developer Studio Project File - Name="game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=game - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "game.mak" CFG="game - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "game - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "game - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Wolfenstein MP/src/game", LTMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W4 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "GAMEDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Omnibot\Common" /i "..\..\..\Omnibot\ET" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /machine:I386 /out:"..\Release/qagame_mp_x86.dll"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "game - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "BUILDING_REF_GL" /D "DEBUG" /D "GAMEDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# SUBTRACT MTL /Oicf
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\Omnibot\Common" /i "..\..\..\Omnibot\ET" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib winmm.lib /nologo /base:"0x20000000" /subsystem:windows /dll /map /debug /machine:I386 /out:"..\Debug/qagame_mp_x86.dll"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "game - Win32 Release"
# Name "game - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\botai\ai_cmd.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmgoal_mp.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmnet_mp.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmq3.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_main.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_script.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_script_actions.c
# End Source File
# Begin Source File

SOURCE=..\botai\ai_team.c
# End Source File
# Begin Source File

SOURCE=.\bg_animation.c
# End Source File
# Begin Source File

SOURCE=.\bg_animgroup.c
# End Source File
# Begin Source File

SOURCE=.\bg_campaign.c
# End Source File
# Begin Source File

SOURCE=.\bg_character.c
# End Source File
# Begin Source File

SOURCE=.\bg_classes.c
# End Source File
# Begin Source File

SOURCE=.\bg_lib.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\bg_malloc.c
# End Source File
# Begin Source File

SOURCE=.\bg_misc.c
# End Source File
# Begin Source File

SOURCE=.\bg_pmove.c
# End Source File
# Begin Source File

SOURCE=.\bg_slidemove.c
# End Source File
# Begin Source File

SOURCE=.\bg_sscript.c
# End Source File
# Begin Source File

SOURCE=.\bg_stats.c
# End Source File
# Begin Source File

SOURCE=.\bg_tracemap.c
# End Source File
# Begin Source File

SOURCE=.\etpro_mdx.c
# End Source File
# Begin Source File

SOURCE=.\g_active.c
# End Source File
# Begin Source File

SOURCE=.\g_alarm.c
# End Source File
# Begin Source File

SOURCE=.\g_antilag.c
# End Source File
# Begin Source File

SOURCE=.\g_bot.c
# End Source File
# Begin Source File

SOURCE=.\g_buddy_list.c
# End Source File
# Begin Source File

SOURCE=.\g_character.c
# End Source File
# Begin Source File

SOURCE=.\g_client.c
# End Source File
# Begin Source File

SOURCE=.\g_cmds.c
# End Source File
# Begin Source File

SOURCE=.\g_cmds_ext.c
# End Source File
# Begin Source File

SOURCE=.\g_combat.c
# End Source File
# Begin Source File

SOURCE=.\g_config.c
# End Source File
# Begin Source File

SOURCE=.\g_etbot_interface.c
# End Source File
# Begin Source File

SOURCE=.\g_fireteams.c
# End Source File
# Begin Source File

SOURCE=.\g_items.c
# End Source File
# Begin Source File

SOURCE=.\g_main.c
# End Source File
# Begin Source File

SOURCE=.\g_match.c
# End Source File
# Begin Source File

SOURCE=.\g_mem.c
# End Source File
# Begin Source File

SOURCE=.\g_misc.c
# End Source File
# Begin Source File

SOURCE=.\g_missile.c
# End Source File
# Begin Source File

SOURCE=.\g_mover.c
# End Source File
# Begin Source File

SOURCE=.\g_multiview.c
# End Source File
# Begin Source File

SOURCE=.\g_props.c
# End Source File
# Begin Source File

SOURCE=.\g_referee.c
# End Source File
# Begin Source File

SOURCE=.\g_save.c
# End Source File
# Begin Source File

SOURCE=.\g_script.c
# End Source File
# Begin Source File

SOURCE=.\g_script_actions.c
# End Source File
# Begin Source File

SOURCE=.\g_session.c
# End Source File
# Begin Source File

SOURCE=.\g_sp_script.c
# End Source File
# Begin Source File

SOURCE=.\g_sp_script_actions.c
# End Source File
# Begin Source File

SOURCE=.\g_spawn.c
# End Source File
# Begin Source File

SOURCE=.\g_stats.c
# End Source File
# Begin Source File

SOURCE=.\g_sv_entities.c
# End Source File
# Begin Source File

SOURCE=.\g_svcmds.c
# End Source File
# Begin Source File

SOURCE=.\g_syscalls.c
# End Source File
# Begin Source File

SOURCE=.\g_systemmsg.c
# End Source File
# Begin Source File

SOURCE=.\g_target.c
# End Source File
# Begin Source File

SOURCE=.\g_team.c
# End Source File
# Begin Source File

SOURCE=.\g_teammapdata.c
# End Source File
# Begin Source File

SOURCE=.\g_tramcar.c
# End Source File
# Begin Source File

SOURCE=.\g_trigger.c
# End Source File
# Begin Source File

SOURCE=.\g_utils.c
# End Source File
# Begin Source File

SOURCE=.\g_vote.c
# End Source File
# Begin Source File

SOURCE=.\g_weapon.c
# End Source File
# Begin Source File

SOURCE=.\g_xpsave.c
# End Source File
# Begin Source File

SOURCE=.\game.def
# End Source File
# Begin Source File

SOURCE=.\npc_ai.c
# End Source File
# Begin Source File

SOURCE=.\npc_air_vehicle.c
# End Source File
# Begin Source File

SOURCE=.\npc_humanoid.c
# End Source File
# Begin Source File

SOURCE=.\npc_humanoid_wp.c
# End Source File
# Begin Source File

SOURCE=.\npc_water_vehicle.c
# End Source File
# Begin Source File

SOURCE=.\om_flagsys.c
# End Source File
# Begin Source File

SOURCE=.\om_spawnsys.c
# End Source File
# Begin Source File

SOURCE=.\q_math.c
# End Source File
# Begin Source File

SOURCE=.\q_shared.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_ai.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_autowaypoint.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_goal.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_items.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_mapping.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_mem.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_movement.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_navigation.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_nodes.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_spawn.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_utils.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_vehicle_ai.c
# End Source File
# Begin Source File

SOURCE=..\uniquebot\unique_visibility.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\botai\ai_cmd.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_distances.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmgoal_mp.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmnet_mp.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_dmq3.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_main.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_matrix.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_team.h
# End Source File
# Begin Source File

SOURCE=..\botai\ai_teamX.h
# End Source File
# Begin Source File

SOURCE=.\bg_classes.h
# End Source File
# Begin Source File

SOURCE=.\bg_local.h
# End Source File
# Begin Source File

SOURCE=.\bg_malloc.h
# End Source File
# Begin Source File

SOURCE=.\bg_public.h
# End Source File
# Begin Source File

SOURCE=..\botai\botai.h
# End Source File
# Begin Source File

SOURCE=.\botlib.h
# End Source File
# Begin Source File

SOURCE=..\botai\chars.h
# End Source File
# Begin Source File

SOURCE=.\etpro_mdx.h
# End Source File
# Begin Source File

SOURCE=.\etpro_mdx_lut.h
# End Source File
# Begin Source File

SOURCE=.\g_etbot_interface.h
# End Source File
# Begin Source File

SOURCE=.\g_local.h
# End Source File
# Begin Source File

SOURCE=.\g_public.h
# End Source File
# Begin Source File

SOURCE=.\g_team.h
# End Source File
# Begin Source File

SOURCE=..\botai\inv.h
# End Source File
# Begin Source File

SOURCE=..\botai\match.h
# End Source File
# Begin Source File

SOURCE=.\q_global_defines.h
# End Source File
# Begin Source File

SOURCE=.\q_shared.h
# End Source File
# Begin Source File

SOURCE=.\surfaceflags.h
# End Source File
# Begin Source File

SOURCE=..\botai\syn.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
