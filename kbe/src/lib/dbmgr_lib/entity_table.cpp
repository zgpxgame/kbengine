/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "entity_table.hpp"
#include "db_interface.hpp"
#include "entitydef/entitydef.hpp"
#include "entitydef/scriptdef_module.hpp"

namespace KBEngine { 
KBE_SINGLETON_INIT(EntityTables);

EntityTables g_EntityTables;

//-------------------------------------------------------------------------------------
EntityTables::EntityTables()
{
}

//-------------------------------------------------------------------------------------
EntityTables::~EntityTables()
{
	tables_.clear();
}

//-------------------------------------------------------------------------------------
bool EntityTables::load(DBInterface* dbi)
{
	pdbi_ = dbi;

	EntityDef::SCRIPT_MODULES smodules = EntityDef::getScriptModules();
	EntityDef::SCRIPT_MODULES::const_iterator iter = smodules.begin();
	for(; iter != smodules.end(); iter++)
	{
		ScriptDefModule* pSM = (*iter).get();
		EntityTable* pEtable = dbi->createEntityTable();
		bool ret = pEtable->initialize(pdbi_, pSM);

		if(!ret)
		{
			delete pEtable;
			return false;
		}

		tables_[pEtable->tableName()].reset(pEtable);
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool EntityTables::syncToDB()
{
	std::vector<std::string> dbTableNames;
	pdbi_->getTableNames(dbTableNames, "");

	// 检查是否有需要删除的表
	std::vector<std::string>::iterator iter0 = dbTableNames.begin();
	for(; iter0 != dbTableNames.end(); iter0++)
	{
		std::string tname = (*iter0);
		if(std::string::npos == tname.find("tbl_"))
			continue;

		KBEngine::kbe_replace(tname, "tbl_", "");
		EntityTables::TABLES_MAP::iterator iter = tables_.find(tname);
		if(iter == tables_.end())
		{
			if(!pdbi_->dropEntityTableFromDB((std::string("tbl_") + tname).c_str()))
				return false;
		}
	}

	// 开始同步所有表
	EntityTables::TABLES_MAP::iterator iter = tables_.begin();
	for(; iter != tables_.end(); iter++)
	{
		if(!iter->second->syncToDB())
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------
}
