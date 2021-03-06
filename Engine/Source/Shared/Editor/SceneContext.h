#pragma once

#include "Base/BcTypes.h"
#include "System/Scene/ScnCoreCallback.h"

#include <set>

namespace Editor
{
	struct SceneContext : public ScnCoreCallback
	{
		std::set< class ScnComponent* > DebugComponents_;

		void onAttachComponent( class ScnComponent* Component ) override;
		void onDetachComponent( class ScnComponent* Component ) override;
	};
}
