#include "EndPoint.h"

mtnet::IPEndPoint::IPEndPoint(const IPAddress& _ipAddress, PortNumber _portNumber) :
	addressInfo_{},
	ipAddress_{ _ipAddress },
	portNumber_{ _portNumber }
{
}

mtnet::IPEndPoint::~IPEndPoint()
{
}

mtnet::IPAddress mtnet::IPEndPoint::GetIPAddress()
{
	return IPAddress();
}

bool mtnet::IPEndPoint::TryGenerateSockAddrIn(SOCKADDR_IN* _addr) const
{
	std::istringstream addressString{ ipAddress_ };
	std::string pickString{};

	_addr->sin_family = AF_INET;
	_addr->sin_port = portNumber_;

	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	_addr->sin_addr.S_un.S_un_b.s_b1 = static_cast<UCHAR>(std::stoul(pickString));
	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	_addr->sin_addr.S_un.S_un_b.s_b2 = static_cast<UCHAR>(std::stoul(pickString));
	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	_addr->sin_addr.S_un.S_un_b.s_b3 = static_cast<UCHAR>(std::stoul(pickString));
	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	_addr->sin_addr.S_un.S_un_b.s_b4 = static_cast<UCHAR>(std::stoul(pickString));

	return true;
}

bool mtnet::IPEndPoint::TryGetOctets(byte* _octet1, byte* _octet2, byte* _octet3, byte* _octet4) const
{
	std::istringstream addressString{ ipAddress_ };
	std::string pickString{};

	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	*_octet1 = static_cast<UCHAR>(std::stoul(pickString));
	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	*_octet2 = static_cast<UCHAR>(std::stoul(pickString));
	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	*_octet3 = static_cast<UCHAR>(std::stoul(pickString));
	if (!std::getline(addressString, pickString, '.'))
	{
		return false;
	}
	*_octet4 = static_cast<UCHAR>(std::stoul(pickString));

	return true;
}
