#include <cybozu/test.hpp>
#include <crypto_util.hpp>
#include <cybozu/benchmark.hpp>
#include "custom_sha1.hpp"

namespace ms {

inline std::string hashPasswordOrg(cybozu::crypto::Hash::Name name, const std::string& salt, const std::string& pass, int spinCount)
{
	cybozu::crypto::Hash s(name);
	std::string h = s.digest(salt + pass);
	assert(h.size() == 20);
	for (int i = 0; i < spinCount; i++) {
		char iter[4];
		cybozu::Set32bitAsLE(iter, i);
		s.update(iter, sizeof(iter));
		s.digest(&h[0], &h[0], h.size());
	}
	return h;
}

inline std::string sha1PasswordNew(const std::string& salt, const std::string& pass, int spinCount)
{
	std::string h = cybozu::crypto::Hash::digest(cybozu::crypto::Hash::N_SHA1, salt + pass);
	assert(h.size() == 20);
	CustomSha1::digest(&h[0], spinCount);
	return h;
}

} // ms

CYBOZU_TEST_AUTO(hashPassword)
{
	cybozu::crypto::Hash::Name hash = cybozu::crypto::Hash::N_SHA1;
	const std::string pass[8] = { "adsfasdf", "xxx", "12345234", "999", "abcD", "XXX", "---", "---------sfas" };
	const std::string salt = "sdfa3rvawvfafas";
	std::string out[8];
	const int spinCount = 10000;
	const std::string x = ms::hashPasswordOrg(hash, salt, pass[0], spinCount);
	const std::string y = ms::sha1PasswordNew(salt, pass[0], spinCount);
	CYBOZU_TEST_EQUAL(x, y);
#ifdef SHA1_USE_SIMD
	const int N = Uint32Vec::size;
	printf("SIMD=%d\n", N);
	ms::sha1PasswordX<N>(out, salt, pass, spinCount);
	for (int i = 0; i < N; i++) {
		CYBOZU_TEST_EQUAL(ms::hex(out[i]), ms::hex(ms::hashPasswordOrg(hash, salt, pass[i], spinCount)));
	}
#endif
#ifdef NDEBUG
	if (x == y) {
		CYBOZU_BENCH("org", ms::hashPasswordOrg, hash, salt, pass[0], spinCount);
		CYBOZU_BENCH("new", ms::sha1PasswordNew, salt, pass[0], spinCount);
#ifdef SHA1_USE_SIMD
		CYBOZU_BENCH("xN ", ms::sha1PasswordX<Uint32Vec::size>,  out, salt, pass, spinCount);
#endif
	}
#endif
}
