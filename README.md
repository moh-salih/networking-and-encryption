# Multi-Layer Cryptography Communication System (KTU Assignment)

This goal of the project is to implement **AES**, **DES**, and **RSA** from scratch, compare it to standard implementations, and integrate 
both implementations in our server-client chat application.

## 🚀 Features

-   **Crypto:** consists of 3 namespaces, `classic` for classic ciphers, `modern` for our implementation of modern algorithms (for now RSA, AES, DES), and `standard` which is basically a wrapper for production-quality implementations by popular libraries (OpenSSL was used).


-   **Dual Implementation Modes:**
    -   **Standard Mode:** Utilizes **OpenSSL** for production-grade security.
    -   **Manual Mode:** Custom-coded AES and DES logic (S-Boxes, Permutations, Round structures) for educational analysis.

-   **Multi-Client Architecture:** Boost.Asio-based server handling multiple concurrent encrypted sessions.
-   **Cross-Platform UI:** -   **Qt-based** Tool for local testing for `crypto`.
    -   **ImGui/GLFW-based** Modern Chat Client and Server Dashboard.

## 🛠 Project Structure

-   `/app/crypto`: Qt GUI for local cipher testing.
-   `/app/net`: Networking logic (Client & Server applications).
-   `/src/crypto`: Core cryptographic logic.
    -   `/classic`: Shift, Vigenere, Playfair, Hill implementations.
    -   `/modern`: Manual AES, DES, and RSA implementations.
    -   `/standard`: OpenSSL wrappers for AES-CBC and RSA-OAEP.
-   `/tests`: Catch2 unit tests for validating implementation correctness.

## 🛡 Network Security & Wireshark Analysis

The system supports two transport layers:
1.  **Plain (TCP):** Data is not encrypted, and is sent in raw JSON format (Visible in Wireshark).
2.  **Secure (TLS/Custom):** Currently broken

## 🔨 Build Instructions

1.  **Requirements:** CMake , OpenSSL, Qt6, Boost (Asio), Catch2, GLFW3, ImGUI

# Disclaimer: AI-assisted tools (including ChatGPT) were widely used during development, primarily for user interfaces and unit tests
