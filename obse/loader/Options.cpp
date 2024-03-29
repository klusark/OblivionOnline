#include "Options.h"

Options g_options;

Options::Options()
:m_launchCS(false)
,m_loadOldblivion(false)
,m_threadTimeout(1000 * 30)
,m_setPriority(false)
,m_priority(0)
{
	//
}

Options::~Options()
{
	//
}

// disable "switch statement contains 'default' but no 'case' labels"
#pragma warning (push)
#pragma warning (disable : 4065)

bool Options::Read(int argc, char ** argv)
{
	if(argc >= 1)
	{
		// remove app name
		argc--;
		argv++;

		int	freeArgCount = 0;

		while(argc > 0)
		{
			char	* arg = *argv++;
			argc--;

			if(arg[0] == '-')
			{
				// switch
				arg++;

				if(!_stricmp(arg, "editor"))
				{
					m_launchCS = true;
				}
				else if(!_stricmp(arg, "old"))
				{
					m_loadOldblivion = true;
				}
				else if(!_stricmp(arg, "priority"))
				{
					if(argc >= 1)
					{
						arg = *argv++;
						argc--;

						m_setPriority = true;

						if(!_stricmp(arg, "above_normal"))
						{
							m_priority = ABOVE_NORMAL_PRIORITY_CLASS;
						}
						else if(!_stricmp(arg, "below_normal"))
						{
							m_priority = BELOW_NORMAL_PRIORITY_CLASS;
						}
						else if(!_stricmp(arg, "high"))
						{
							m_priority = HIGH_PRIORITY_CLASS;
						}
						else if(!_stricmp(arg, "idle"))
						{
							m_priority = IDLE_PRIORITY_CLASS;
						}
						else if(!_stricmp(arg, "normal"))
						{
							m_priority = NORMAL_PRIORITY_CLASS;
						}
						else if(!_stricmp(arg, "realtime"))
						{
							m_priority = REALTIME_PRIORITY_CLASS;
						}
						else
						{
							m_setPriority = false;

							_ERROR("couldn't read priority argument (%s)", arg);
							return false;
						}
					}
					else
					{
						_ERROR("priority not specified");
						return false;
					}
				}
				else if(!_stricmp(arg, "altexe"))
				{
					if(argc >= 1)
					{
						m_altEXE = *argv++;
						argc--;
					}
					else
					{
						_ERROR("exe path not specified");
						return false;
					}
				}
				else if(!_stricmp(arg, "altdll"))
				{
					if(argc >= 1)
					{
						m_altDLL = *argv++;
						argc--;
					}
					else
					{
						_ERROR("dll path not specified");
						return false;
					}
				}
				else if(!_stricmp(arg, "notimeout"))
				{
					m_threadTimeout = INFINITE;
				}
				else
				{
					_ERROR("unknown switch (%s)", arg);
					return false;
				}
			}
			else
			{
				// free arg

				switch(freeArgCount)
				{
					default:
						_ERROR("too many free args (%s)", arg);
						return false;
				}
			}
		}
	}

	return Verify();
}

#pragma warning (pop)

void Options::PrintUsage(void)
{
	gLog.SetPrintLevel(IDebugLog::kLevel_VerboseMessage);

	_MESSAGE("usage: obse_loader [options]");
	_MESSAGE("");
	_MESSAGE("options:");
	_MESSAGE("  -editor - launch the construction set");
	_MESSAGE("  -old - load oldblivion");
	_MESSAGE("  -priority <level> - set the launched program\'s priority");
	_MESSAGE("    above_normal");
	_MESSAGE("    below_normal");
	_MESSAGE("    high");
	_MESSAGE("    idle");
	_MESSAGE("    normal");
	_MESSAGE("    realtime");
	_MESSAGE("  -altexe <path> - set alternate exe path");
	_MESSAGE("  -altdll <path> - set alternate dll path");
	_MESSAGE("  -notimeout - wait forever for oblivion to launch");
	_MESSAGE("               this overrides the default five second timeout");
}

bool Options::Verify(void)
{
	// nothing to verify currently

	return true;
}
