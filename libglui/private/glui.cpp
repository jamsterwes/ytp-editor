#include "public/glui.h"
#include "private/menu.h"
#include "private/window.h"
#include <ShObjIdl.h>
#include <codecvt>

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

LPWSTR from_string(std::string str) {
	// Get wstring
	std::wstring wstr = converter.from_bytes(str);

	// Alloc LPWSTR
	wchar_t* lpsz = new wchar_t[wstr.size() + 1]{0};
	wcsncpy(lpsz, wstr.c_str(), wstr.size());

	// Return LPWSTR
	return lpsz;
}

std::string to_string(LPWSTR str) {
	// Get string
	return converter.to_bytes(str);
}

namespace glui {
	// CLASS

	class Context : public IContext {
	public:
		Context();
		~Context();
		IWindow* newWindow(std::string title, int width, int height) override;
		IMenu* newMenu() override;
		void pollEvents() override;
		std::string openFileDialog(std::initializer_list<std::pair<std::string, std::string>> types) override;
	private:
	};

	// IMPLEMENTATION

	Context::Context() {
		// Initialize COM
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	}

	Context::~Context() {
		// Uninitialize COM
		CoUninitialize();
	}

	IWindow* Context::newWindow(std::string title, int width, int height) {
		return new Window(title, width, height);
	}

	IMenu* Context::newMenu() {
		return new Menu();
	}

	void Context::pollEvents() {
		MSG msg;
		GetMessage(&msg, nullptr, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	std::string Context::openFileDialog(std::initializer_list<std::pair<std::string, std::string>> types) {
		std::string selected = "";
		IFileOpenDialog* pFileOpen;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
		if (SUCCEEDED(hr)) {
			UINT fileTypesCount = types.size();
			if (fileTypesCount > 0) {
				COMDLG_FILTERSPEC* specs = new COMDLG_FILTERSPEC[fileTypesCount]{};
				int i = 0;
				for (std::pair<std::string, std::string> type : types) {
					if (i >= fileTypesCount) break;
					specs[i].pszName = from_string(type.first);
					specs[i].pszSpec = from_string(type.second);
					i++;
				}
				hr = pFileOpen->SetFileTypes(fileTypesCount, specs);
				hr = pFileOpen->Show(NULL);

				if (SUCCEEDED(hr)) {
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr)) {
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr)) {
							selected = to_string(pszFilePath);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}

				for (int i = 0; i < fileTypesCount; i++) {
					delete[] specs[i].pszName;
					delete[] specs[i].pszSpec;
				}
				delete[] specs;
			}
			pFileOpen->Release();
		}
		return selected;
	}

	// INIT FUNCTION

	GLUI_API IContext* init() {
		return new Context();
	}

	GLUI_API void destroy(IContext* ctx) {
		delete ctx;
	}
}