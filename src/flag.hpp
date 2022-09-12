#ifndef REDNIGHTMARE_FLAG_HPP
#define REDNIGHTMARE_FLAG_HPP
#include "entity.hpp"
#include "tz/core/vector.hpp"

namespace game
{
	enum class FlagID
	{
		CustomScale,
		Player
	};

	template<FlagID ID>
	struct FlagParams{};

	class IFlag
	{
	public:
		virtual constexpr FlagID get_id() const = 0;
	};

	template<FlagID ID>
	class Flag : public IFlag
	{
	public:
		Flag(FlagParams<ID> params): params(params){}
		virtual constexpr FlagID get_id() const override{return ID;}
		const FlagParams<ID>& data() const{return this->params;}
		FlagParams<ID>& data(){return this->params;}
	private:
		FlagParams<ID> params;
	private:
	};
	
	// Flag implementations.
	template<>
	struct FlagParams<FlagID::CustomScale>
	{
		tz::Vec2 scale;
	};

	// Represents something that stores flags.
	class FlagEntity : public Entity<FlagID, IFlag, Flag, FlagParams>
	{

	};
}

#endif // REDNIGHTMARE_FLAG_HPP
