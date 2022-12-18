﻿#include "DeviceResources.h"
#include "StepTimer.h"

#include <memory>
#include <string>

#ifndef SAMPLEFPSTEXTRENDERER_H_9238679823
#define SAMPLEFPSTEXTRENDERER_H_9238679823

namespace the_one_uwp
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	struct SampleFpsTextRenderer {
		std::shared_ptr<DX::D3DDeviceResources>			DeviceResources					= {};	// Cached pointer to device resources.

		// Resources related to text rendering.
		std::wstring									Text							= {};
		DWRITE_TEXT_METRICS								TextMetrics						= {};
		::gpk::ptr_com<ID2D1SolidColorBrush>			WhiteBrush						= {};
		::gpk::ptr_com<ID2D1DrawingStateBlock1>			StateBlock						= {};
		::gpk::ptr_com<IDWriteTextLayout3>				TextLayout						= {};
		::gpk::ptr_com<IDWriteTextFormat2>				TextFormat						= {};

		void											CreateDeviceDependentResources	() { DX::ThrowIfFailed(DeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &WhiteBrush)); }
		void											ReleaseDeviceDependentResources	() { WhiteBrush = {}; }

		// Initializes D2D resources used for text rendering.
														SampleFpsTextRenderer			(const std::shared_ptr<DX::D3DDeviceResources>& deviceResources) : DeviceResources(deviceResources) {
			// Create device independent resources
			::gpk::ptr_com<IDWriteTextFormat>			textFormat						= {};
			DX::ThrowIfFailed(DeviceResources->GetDWriteFactory()->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-US", &textFormat));
			textFormat.as(TextFormat);
			DX::ThrowIfFailed(TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));
			DX::ThrowIfFailed(DeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&StateBlock));

			CreateDeviceDependentResources();
		}

		// Updates the text to be displayed.
		void											Update							(DX::StepTimer const& timer) {
			// Update display text.
			uint32_t											fps								= timer.GetFramesPerSecond();
			Text											= (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

			::gpk::ptr_com<IDWriteTextLayout>			textLayout;
			constexpr float										w								= 240.0f; // Max width of the input text.
			constexpr float										h								= 50.0f;// Max height of the input text.
			DX::ThrowIfFailed(DeviceResources->GetDWriteFactory()->CreateTextLayout(Text.c_str(), (uint32) Text.length(), TextFormat, w, h, &textLayout));
			textLayout.as(TextLayout);
			DX::ThrowIfFailed(TextLayout->GetMetrics(&TextMetrics));
		}

		// Renders a frame to the screen.
		void											Render							() {
			ID2D1DeviceContext									* context						= DeviceResources->GetD2DDeviceContext();
			const ::gpk::SCoord2<float>							logicalSize						= DeviceResources->GetLogicalSize();

			context->SaveDrawingState(StateBlock);
			context->BeginDraw();

			// Position on the bottom right corner
			D2D1::Matrix3x2F									screenTranslation				= D2D1::Matrix3x2F::Translation(logicalSize.x - TextMetrics.layoutWidth, logicalSize.y - TextMetrics.height);
			context->SetTransform(screenTranslation * DeviceResources->GetOrientationTransform2D());
			DX::ThrowIfFailed(TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING));

			context->DrawTextLayout(D2D1::Point2F(0.f, 0.f), TextLayout, WhiteBrush);
	
			HRESULT												hr								= context->EndDraw();
			if (hr != D2DERR_RECREATE_TARGET)	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device is lost. It will be handled during the next call to Present.
				DX::ThrowIfFailed(hr);

			context->RestoreDrawingState(StateBlock);
		}

	};
}

#endif // SAMPLEFPSTEXTRENDERER_H_9238679823