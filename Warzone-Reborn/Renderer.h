#ifndef __ZIRCO_H__
#define __ZIRCO_H__

#include <stdint.h>

typedef uint64_t uint150_t;

namespace Renderer
{
	struct Status
	{
		enum Enum
		{
			UnknownError			= -1,
			NotSupportedError		= -2,
			ModuleNotFoundError		= -3,

			AlreadyInitializedError = -4,
			NotInitializedError		= -5,

			Success					= 0,
		};
	};

	struct RenderType
	{
		enum Enum
		{
			None,
			D3D12
		};
	};

	Status::Enum	init(RenderType::Enum renderType);
	void			shutdown();

	Status::Enum	bind(uint16_t index, void** original, void* function);
	void			unbind(uint16_t index);

	RenderType::Enum getRenderType();
	uint150_t*		 getMethodsTable();
}
#endif // __ZIRCO_H__