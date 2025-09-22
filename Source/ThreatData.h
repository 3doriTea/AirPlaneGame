#pragma once

/// <summary>
/// ‹ºˆĞƒŒƒxƒ‹‚ğ•\‚·—ñ‹“Œ^
/// </summary>
enum class ThreatLevel
{
	Normal,// ’Êíó‘Ô
	Danger // ŠëŒ¯ó‘Ô
};

struct ThreatEventData
{
	ThreatLevel level;
};