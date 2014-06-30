// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Slate.h"
#include "SCurveEditor.h"

/**
 * A Slate SpinBox resembles traditional spin boxes in that it is a widget that provides
 * keyboard-based and mouse-based manipulation of a numberic value.
 * Mouse-based manipulation: drag anywhere on the spinbox to change the value.
 * Keyboard-based manipulation: click on the spinbox to enter text mode.
 */
DECLARE_DELEGATE_TwoParams( FOnCropAnimSequence, bool, float )
DECLARE_DELEGATE_TwoParams( FOnScrubBarDrag, int32, float)

class KISMETWIDGETS_API SScrubWidget : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SScrubWidget)
		: _Value(0)
		, _NumOfKeys(10)
		, _SequenceLength()
		, _bAllowZoom(false)
		, _OnValueChanged()
		, _OnBeginSliderMovement()
		, _OnEndSliderMovement()
		, _ViewInputMax()
		, _ViewInputMin()
		, _OnSetInputViewRange()
		, _OnCropAnimSequence()
		, _bLastFrameIsFirstFrame(true)
		{}
		
		/** The value to display */
		SLATE_ATTRIBUTE( float, Value )
		SLATE_ATTRIBUTE( uint32, NumOfKeys )
		SLATE_ATTRIBUTE( float, SequenceLength )
		SLATE_ARGUMENT( bool, bAllowZoom )
		/** Called when the value is changed by slider or typing */
		SLATE_EVENT( FOnFloatValueChanged, OnValueChanged )
		/** Called right before the slider begins to move */
		SLATE_EVENT( FSimpleDelegate, OnBeginSliderMovement )
		/** Called right after the slider handle is released by the user */
		SLATE_EVENT( FOnFloatValueChanged, OnEndSliderMovement )
		/** View Input range **/
		SLATE_ATTRIBUTE( float, ViewInputMin )
		SLATE_ATTRIBUTE( float, ViewInputMax )
		SLATE_EVENT( FOnSetInputViewRange, OnSetInputViewRange )
		/** Called when an anim sequence is cropped before/after a selected frame */
		SLATE_EVENT( FOnCropAnimSequence, OnCropAnimSequence )
		/** Called to zero out selected frame's translation from origin */
		SLATE_EVENT( FSimpleDelegate, OnReZeroAnimSequence )
		/** Optional, additional values to draw on the timeline **/
		SLATE_ATTRIBUTE( TArray<float>, DraggableBars )
		SLATE_EVENT( FOnScrubBarDrag, OnBarDrag)
		SLATE_ARGUMENT( bool, bLastFrameIsFirstFrame )
	SLATE_END_ARGS()


	/**
	 * Construct the widget
	 * 
	 * @param InArgs   A declaration from which to construct the widget
	 */
	void Construct( const FArguments& InArgs );

	// SWidget interface
	virtual FVector2D ComputeDesiredSize() const override;
	virtual int32 OnPaint( const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

	static int32 GetDivider(float InputMinX, float InputMaxX, FVector2D WidgetSize, float SequenceLength, int32 NumFrames, bool bLastFrameIsFirstFrame = true);
protected:

	/**
	 * Call this method when the user's interaction has changed the value
	 *
	 * @param NewValue				Value resulting from the user's interaction
	 * @param bSliderClamp			true to clamp the slider value to the nearest delta value
	 * @param bCommittedFromText	true if the value was committed from the text box and not the slider	
	 */
	void CommitValue( float NewValue, bool bSliderClamp, bool bCommittedFromText );

private:

	/** Function to create context menu to display anim sequence editing options */
	void CreateContextMenu( float CurrentFrameTime );

	/** Function to crop animation sequence before/after selected frame */
	void OnSequenceCropped( bool bFromStart, float CurrentFrameTime );

	/** Function to zero out translation of the selected frame */
	void OnReZero();

	TAttribute<float> ValueAttribute;
	FOnFloatValueChanged OnValueChanged;
	FSimpleDelegate OnBeginSliderMovement;
	FOnFloatValueChanged OnEndSliderMovement;

	TAttribute<float> ViewInputMin;
	TAttribute<float> ViewInputMax;
	FOnSetInputViewRange OnSetInputViewRange;
	FOnCropAnimSequence OnCropAnimSequence;
	FSimpleDelegate		OnReZeroAnimSequence;

	/** Dragagble bars are generic lines drawn on the scrub widget that can be dragged with the mouse. This is very bare bones and just represents drawing/moving float values */
	TAttribute<TArray<float>>	DraggableBars;
	FOnScrubBarDrag OnBarDrag;

	/** Distance Dragged **/
	float DistanceDragged;
	/** Number of Keys **/
	TAttribute<uint32> NumOfKeys;
	TAttribute<float> SequenceLength;
	/** The number of decimal places to display */
	bool bDragging;
	bool bAllowZoom;	
	/** If we are currently panning the panel*/
	bool bPanning;
	/** Has the mouse moved during panning - used to determine if we should open the context menu or not */
	bool bMouseMovedDuringPanning;
	/** Bar Dragging*/
	int32 DraggableBarIndex;
	bool DraggingBar;

	bool bLastFrameIsFirstFrame;
};

