#include "stdafx.h"
#include "FileSystemUtil.h"
#include "LogFactory.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

bool CreateFolderIfNotExists(const string& folderName)
{
	path p(folderName);

	path currDir;
	for (path::iterator iter = p.begin(); iter != p.end(); ++iter)
	{
		currDir /= (*iter);
		if (!exists(currDir))
		{
			try
			{
				create_directory(currDir);
			}
			catch (const filesystem_error& ex)
			{
				CLogFactory::GetInstance().GetLogger("main").error(ex.what());
				return false;
			}
		}
	}

	return true;
}
