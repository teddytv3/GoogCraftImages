#include "pch.h"
#include "CppUnitTest.h"

#include "../Client/Commands.h"
#include <defines.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(Socket)
	{
	public:

		TEST_METHOD(SOCKET_001)
		{
			Shared::Socket sock;
			sock.open();
			sock.connect(2000, "2.3.3.3");
			
			// A failure should be raised 
			Assert::AreEqual(true, sock.fail());
		}

		TEST_METHOD(SOCKET_002)
		{
			//Shared::Socket 
		}

		TEST_METHOD(SOCKET_003)
		{

		}
		TEST_METHOD(SOCKET_004)
		{

		}
	};
}
