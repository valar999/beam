#include "beam/node.h"
#include "utility/logger.h"
#include "tools/base_node_connection.h"
#include "tools/tx_generator.h"

#include <vector>
#include <thread>
#include <future>

using namespace beam;
using namespace ECC;

class TestNodeConnection : public BaseTestNodeConnection
{
public:
	TestNodeConnection(int argc, char* argv[]);
private:
	
	virtual void OnClosed(int errorCode) override;
	virtual void GenerateTests() override;
};

TestNodeConnection::TestNodeConnection(int argc, char* argv[])
	: BaseTestNodeConnection(argc, argv)
{
	m_Timeout = 0;
}

void TestNodeConnection::OnClosed(int)
{
	LOG_INFO() << "Ok: connection is reset";
	io::Reactor::get_Current().stop();
}

void TestNodeConnection::GenerateTests()
{
	m_Tests.push_back([this]()
	{
		LOG_INFO() << "Send big transaction";
		TxGenerator gen(m_Kdf);

		Amount amount = 20000;

		// Inputs
		gen.GenerateInputInTx(1, amount);

		// Outputs
		for (Amount i = 0; i < amount; ++i)
		{
			gen.GenerateOutputInTx(1, 1);
		}

		// Kernels
		gen.GenerateKernel(1);

		gen.Sort();

		Send(gen.GetTransaction());
	});
}

int main(int argc, char* argv[])
{
	int logLevel = LOG_LEVEL_DEBUG;
#if LOG_VERBOSE_ENABLED
	logLevel = LOG_LEVEL_VERBOSE;
#endif
	auto logger = Logger::create(logLevel, logLevel);

	TestNodeConnection connection(argc, argv);

	connection.Run();

	return connection.CheckOnFailed();	
}