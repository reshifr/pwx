#include <winhello.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <webauthn.h>
#include <sodium.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int winhello(void) {

  HWND hWnd = GetForegroundWindow();

  WEBAUTHN_RP_ENTITY_INFORMATION rpInfo = {};
  rpInfo.dwVersion = WEBAUTHN_RP_ENTITY_INFORMATION_CURRENT_VERSION;
  rpInfo.pwszId = L"pwdex";
  rpInfo.pwszName = L"pwdex";
  rpInfo.pwszIcon = nullptr;

  std::vector<BYTE> userId = {0x01, 0x02, 0x03, 0x04};
  WEBAUTHN_USER_ENTITY_INFORMATION userInfo = {};
  userInfo.dwVersion = WEBAUTHN_USER_ENTITY_INFORMATION_CURRENT_VERSION;
  userInfo.cbId = userId.size();
  userInfo.pbId = userId.data();
  userInfo.pwszName = L"renol";
  userInfo.pwszIcon = nullptr;
  userInfo.pwszDisplayName = L"Renol P. H.";

  WEBAUTHN_COSE_CREDENTIAL_PARAMETER credentialParams = {};
  credentialParams.dwVersion =
      WEBAUTHN_COSE_CREDENTIAL_PARAMETER_CURRENT_VERSION;
  credentialParams.pwszCredentialType = WEBAUTHN_CREDENTIAL_TYPE_PUBLIC_KEY;
  credentialParams.lAlg = WEBAUTHN_COSE_ALGORITHM_ECDSA_P256_WITH_SHA256;

  WEBAUTHN_COSE_CREDENTIAL_PARAMETERS pubKeyCredParams = {};
  pubKeyCredParams.cCredentialParameters = 1;
  pubKeyCredParams.pCredentialParameters = &credentialParams;

  std::string clientDataJSON = "{\"type\":\"webauthn.create\",\"challenge\":"
                               "\"Y2hhbGxlbmdl\",\"origin\":\"http://pwdex\"}";
  WEBAUTHN_CLIENT_DATA webAuthNClientData = {};
  webAuthNClientData.dwVersion = WEBAUTHN_CLIENT_DATA_CURRENT_VERSION;
  webAuthNClientData.cbClientDataJSON = (DWORD)clientDataJSON.size();
  webAuthNClientData.pbClientDataJSON = (PBYTE)clientDataJSON.c_str();
  webAuthNClientData.pwszHashAlgId = WEBAUTHN_HASH_ALGORITHM_SHA_256;

  WEBAUTHN_AUTHENTICATOR_MAKE_CREDENTIAL_OPTIONS options = {};
  options.dwVersion =
      WEBAUTHN_AUTHENTICATOR_MAKE_CREDENTIAL_OPTIONS_CURRENT_VERSION;
  options.dwTimeoutMilliseconds = 60000;
  options.dwAuthenticatorAttachment =
      WEBAUTHN_USER_VERIFICATION_REQUIREMENT_ANY;
  options.bRequireResidentKey = TRUE;
  options.dwUserVerificationRequirement =
      WEBAUTHN_USER_VERIFICATION_REQUIREMENT_REQUIRED;
  options.dwAttestationConveyancePreference =
      WEBAUTHN_ATTESTATION_CONVEYANCE_PREFERENCE_DIRECT;
  options.bEnablePrf = TRUE;
  options.pCancellationId = nullptr;
  options.pExcludeCredentialList = nullptr;

  PWEBAUTHN_CREDENTIAL_ATTESTATION pAttestation = nullptr; // Penampung kado

  printf("Memanggil Windows Hello... 2\n");

  HRESULT hr = WebAuthNAuthenticatorMakeCredential(
      hWnd, &rpInfo, &userInfo, &pubKeyCredParams, &webAuthNClientData,
      &options,     // Argumen ke-6
      &pAttestation // Argumen ke-7 (Hasilnya di sini)
  );

  if (SUCCEEDED(hr)) {
    printf("MANTAP BOSQ! Berhasil.\n");
    printf("Credential ID Size: %d\n", pAttestation->cbCredentialId);

    // JANGAN LUPA DIBERSIHIN
    WebAuthNFreeCredentialAttestation(pAttestation);
  } else {
    printf("GAGAL total! HRESULT: 0x%X\n", hr);
  }

  return 0;
}

int winhello2(void) {
  // 1. Inisialisasi Libsodium
  if (sodium_init() < 0)
    return 1;

  // 2. Simulasi data BINARY (Misal: Public Key atau Signature)
  unsigned char raw_data[32];
  randombytes_buf(raw_data, sizeof raw_data); // Isi data acak

  // 3. ENCODE: Binary -> Base64 (Text)
  // Libsodium punya fungsi sodium_bin2base64 yang aman dan kencang
  char b64_output[64];
  sodium_bin2base64(b64_output, sizeof b64_output, raw_data, sizeof raw_data,
                    sodium_base64_VARIANT_ORIGINAL);

  std::cout << "--- DATA ASLI (HEX) ---" << std::endl;
  // (Cuma buat liat isi aslinya)

  // 4. MASUKIN KE JSON
  json webauthn_packet;
  webauthn_packet["username"] = "Renol_Cyber";
  webauthn_packet["challenge_b64"] = b64_output; // Base64 masuk sini
  webauthn_packet["algorithm"] = "ED25519";

  // Serialize JSON ke String buat dikirim/disimpan
  std::string json_result = webauthn_packet.dump(4);
  std::cout << "\n--- HASIL JSON ---" << std::endl;
  std::cout << json_result << std::endl;

  // 5. DECODE: Base64 (Text) -> Binary kembali
  // Misalnya kita nerima JSON dan mau ambil datanya lagi
  std::string b64_input = webauthn_packet["challenge_b64"];
  unsigned char decoded_bin[32];
  size_t bin_len;

  sodium_base642bin(decoded_bin, sizeof decoded_bin, b64_input.c_str(),
                    b64_input.length(), nullptr, &bin_len, nullptr,
                    sodium_base64_VARIANT_ORIGINAL);

  std::cout << "\n--- DECODE BERHASIL ---" << std::endl;
  std::cout << "Data kembali ke ukuran: " << bin_len << " bytes" << std::endl;

  return 0;
}
