// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OnlineSubsystemTypes.h"
#include "OnlineChatInterface.h"

#define F_PREFIX(TypeToPrefix) F##TypeToPrefix
#define PARTY_DECLARE_DELEGATETYPE(Type) typedef F##Type::FDelegate F##Type##Delegate

/**
 * Key value data that can be sent to party members
 */
typedef FOnlineKeyValuePairs<FString, FVariantData> FOnlinePartyAttrs;

/**
 * Party member user info returned by IOnlineParty interface
 */
class FOnlinePartyMember : public FOnlineUser
{
public:
};

/**
 * Data associated with the entire party
 */
class FOnlinePartyData : public TSharedFromThis<FOnlinePartyData>
{
public:
	FOnlinePartyData() {}
	virtual ~FOnlinePartyData() {}

	/**
	 * Get an attribute from the party data
	 *
	 * @param AttrName - key for the attribute
	 * @param OutAttrValue - [out] value for the attribute if found
	 *
	 * @return true if the attribute was found
	 */
	bool GetAttribute(const FString& AttrName, FVariantData& OutAttrValue) const
	{
		bool bResult = false;

		const FVariantData* FoundValuePtr = KeyValAttrs.Find(AttrName);
		if (FoundValuePtr != nullptr)
		{
			OutAttrValue = *FoundValuePtr;
		}

		return bResult;
	}

	/**
	 * Set an attribute from the party data
	 *
	 * @param AttrName - key for the attribute
	 * @param AttrValue - value to set the attribute to
	 */
	void SetAttribute(const FString& AttrName, const FVariantData& AttrValue)
	{
		FVariantData& NewAttrValue = KeyValAttrs.FindOrAdd(AttrName);
		NewAttrValue = AttrValue;
	}

	void ToJson(FString& JsonString) const;
	void FromJson(const FString& JsonString);

	/**
	 * Dump state about the party data for debugging
	 */
	virtual FString DumpDebug() const
	{
		FString Result;
		const FOnlinePartyAttrs& PartyAttrs = KeyValAttrs;
		for (FOnlinePartyAttrs::TConstIterator It(PartyAttrs); It; ++It)
		{
			FString Key = It.Key();
			const FVariantData& Value = It.Value();
			Result += FString::Printf(TEXT("\n%s=%s"), *Key, *Value.ToString());
		}
		return Result;
	}

	/** map of key/val attributes that represents the data */
	FOnlinePartyAttrs  KeyValAttrs;
};

/**
 * Info needed to join a party
 */
class IOnlinePartyJoinInfo
{
public:
	IOnlinePartyJoinInfo() {}
	virtual ~IOnlinePartyJoinInfo() {}

	/**
	 * @return id of the user this invite is from
	 */
	virtual const TSharedRef<const FUniqueNetId>& GetLeaderId() const = 0;

	/**
	 * @return id of the party associated with this invite
	 */
	virtual const TSharedRef<const FOnlinePartyId>& GetPartyId() const = 0;

	/**
	 * @return id of the client app associated with the sender of the party invite 
	 */
	virtual const FString& GetAppId() const = 0;

	/**
	 * @return id of the client app associated with the sender of the party invite 
	 */
	virtual int32 GetBuildId() const = 0;

	/**
	 * @return whether or not the party is accepting members
	 */
	virtual bool GetIsAcceptingMembers() const = 0;

	/**
	 * @return why the party is not accepting members
	 */
	virtual int32 GetNotAcceptingReason() const = 0;

	/**
	 * @return true if the join info cannot be used to join a party
	 */
	virtual bool IsInvalidForJoin() const = 0;

	/**
	 * @return true if the join info cannot be used to request an invite
	 */
	virtual bool IsInvalidForInviteRequest() const = 0;

	/**
	 * @return the game-specific custom data that was sent along with this invite
	 */
	virtual const FOnlinePartyData& GetCustomPayload() const = 0;
};

/**
 * The binding of a join info object to a given sender
 */
class IOnlinePartyInvite
{
public:
	IOnlinePartyInvite() {}
	virtual ~IOnlinePartyInvite() {}

	virtual const TSharedRef<const FUniqueNetId>& GetSenderId() const = 0;
	virtual const TSharedRef<IOnlinePartyJoinInfo>& GetJoinInfo() const = 0;
};

/**
 * Info needed to join a party
 */
class IOnlinePartyPendingJoinRequestInfo
{
public:
	IOnlinePartyPendingJoinRequestInfo() {}
	virtual ~IOnlinePartyPendingJoinRequestInfo() {}

	/**
	* @return id of the party associated with this invite
	*/
	virtual const TSharedRef<const FOnlinePartyId>& GetPartyId() const = 0;

	/**
	 * @return id of the sender of this join request
	 */
	virtual const TSharedRef<const FUniqueNetId>& GetSenderId() const = 0;

	/**
	 * @return display name of the sender of this join request
	 */
	virtual const FString& GetSenderDisplayName() const = 0;
};

/**
 * Permissions for joining a party
 */
namespace EPartyPermissions
{
	enum Type
	{
		/** any user that discovers the party can join */
		Public,
		/** invite required to join the party */
		Private
	};

	inline const TCHAR* ToString(EPartyPermissions::Type PartyPermissions)
	{
		switch (PartyPermissions)
		{
		case Public:
		{
			return TEXT("Public");
		}
		case Private:
		{
			return TEXT("Private");
		}
		}
		return TEXT("");
	}
}

/**
 * Options for configuring a new party or for updating an existing party
 */
struct FPartyConfiguration : public TSharedFromThis<FPartyConfiguration>
{
	FPartyConfiguration()
		: Permissions(EPartyPermissions::Public)
		, bPresenceParty(false)
		, bShouldPublishToPresence(false)
		, bCanNonLeaderPublishToPresence(false)
		, bCanNonLeaderInviteToParty(false)
		, bShouldRemoveOnDisconnection(false)
		, bIsAcceptingMembers(false)
		, NotAcceptingMembersReason(0)
		, MaxMembers(0)
	{}

	/** Permission for configuring party */
	EPartyPermissions::Type Permissions;
	/** is this party tied to presence */
	bool bPresenceParty;
	/** should publish info to presence */
	bool bShouldPublishToPresence;
	/** should publish info to presence */
	bool bCanNonLeaderPublishToPresence;
	/** can non-leaders send an invite to people */
	bool bCanNonLeaderInviteToParty;
	/** should remove on disconnection */
	bool bShouldRemoveOnDisconnection;
	/** is accepting members */
	bool bIsAcceptingMembers;
	/** not accepting members reason */
	int32 NotAcceptingMembersReason;
	/** clients can add whatever data they want for configuration options */
	FOnlinePartyData ClientConfigData;
	/** Maximum active members allowed. 0 means no maximum. */
	int32 MaxMembers;
	/** Human readable nickname */
	FString Nickname;
	/** Human readable description */
	FString Description;
	/** Human readable password for party. */
	FString Password;

	FString DumpState() const
	{
		return FString::Printf(TEXT("Permissions: %s PresenceEnabled: %d Publish: L(%d) NL(%d) NL Invite(%d) RemoveOnDisconnect: %d Accepting: %d Not Accepting Reason(%d) MaxMembers: %d Nickname: %s Description: %s Password: %s"),
			ToString(Permissions),
			bPresenceParty,
			bShouldPublishToPresence,
			bCanNonLeaderPublishToPresence,
			bCanNonLeaderInviteToParty,
			bShouldRemoveOnDisconnection,
			bIsAcceptingMembers,
			NotAcceptingMembersReason,
			MaxMembers,
			*Nickname,
			*Description,
			Password.IsEmpty() ? TEXT("not set") : TEXT("set"));
	}
};

enum class EPartyState
{
	None,
	CreatePending,
	DestroyPending,
	JoinPending,
	JoinChatPending,         // to be removed
	JoinFinalizationPending, // to be removed
	LeavePending,
	RemovalPending,
	Active,
	Disconnected,
	CleanUp
};

/**
 * Current state associated with a party
 */
class FOnlinePartyInfo : public TSharedFromThis<FOnlinePartyInfo>
{
	FOnlinePartyInfo();
public:
	FOnlinePartyInfo(const TSharedRef<const FOnlinePartyId>& InPartyId)
		: PartyId(InPartyId)
		, State(EPartyState::None)
		, Config(MakeShareable(new FPartyConfiguration()))
	{}

	virtual ~FOnlinePartyInfo() 
	{}

	virtual TSharedPtr<const FUniqueNetId> GetLeaderId() const = 0;
	
	/**
	 * Return a structure suitable for joining this party
	 */
	virtual const TSharedRef<IOnlinePartyJoinInfo> GetJoinInfo() const = 0;

	/**
	 * Calculate the possible joinability state of this party
	 * check the values from left to right in order of precedence
	 *
	 * @param LocalUserId local user making this request
	 * @param bPublicJoinable [out] is the party joinable by anyone at all
	 * @param bFriendJoinable [out] is the party joinable by friends via presence (doesn't require invite)
	 * @param bInviteOnly [out] is the party joinable via explicit invites
	 * @param bAllowInvites [out] are invites possible (use with bInviteOnly to determine if invites are available right now)
	 *
	 * @return true if the out params are valid, false otherwise
	 */
	virtual bool GetJoinability(const FUniqueNetId& LocalUserId, bool& bPublicJoinable, bool& bFriendJoinable, bool& bInviteOnly, bool& bAllowInvites) const = 0;

	/**
	 * Dump state about the party info for debugging
	 */
	virtual FString DumpState() const = 0;

	/** unique id of the party */
	TSharedRef<const FOnlinePartyId> PartyId;
	/** The current state of the party */
	EPartyState State;
	/** Current state of configuration */
	TSharedRef<FPartyConfiguration> Config;
	/** id of chat room associated with the party */
	FChatRoomId RoomId;
};

enum class EMemberChangedReason
{
	Disconnected,
	Rejoined,
	Promoted
};

enum class EMemberExitedReason
{
	Unknown,
	Left,
	Removed,
	Disbanded,
	Kicked
};

enum class ECreatePartyCompletionResult
{
	UnknownClientFailure = -100,
	AlreadyCreatingParty,
	AlreadyInParty,
	FailedToCreateMucRoom,
	NoResponse,
	UnknownInternalFailure = 0,
	Succeeded = 1
};

enum class EJoinPartyCompletionResult
{
	UnknownClientFailure = -100,
	BadBuild,
	JoinInfoInvalid,
	AlreadyJoiningParty,
	AlreadyInParty,
	UnknownLocalUser,
	FailedToJoinChatRoom,
	MessagingFailure,
	NoSpace,
	NotApproved,
	RequesteeNotMember,
	RequesteeNotLeader,
	NoResponse,
	UnknownTransportFailure,
	UnknownInternalFailure = 0,
	Succeeded = 1
};

enum class ELeavePartyCompletionResult
{
	UnknownClientFailure = -100,
	AlreadyLeavingParty,
	UnknownLocalUser,
	UnknownParty,
	MessagingFailure,
	NoResponse,
	UnknownTransportFailure,
	UnknownInternalFailure = 0,
	Succeeded = 1
};

enum class ESendInviteCompleteResult
{
	NotLoggedIn = -100,
	InvitePending,
	AlreadyInParty,
	PartyFull,
	NoPermission,
	UnknownInternalFailure = 0,
	Succeeded = 1
};

enum class EKickMemberCompletionResult
{
	UnknownClientFailure = -100,
	UnknownParty,
	LocalMemberNotMember,
	LocalMemberNotLeader,
	RemoteMemberNotMember,
	MessagingFailure,
	NoResponse,
	UnknownInternalFailure = 0,
	Succeeded = 1
};

enum class EInvitationResponse;

// Completion delegates

/**
 * Party creation async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully created new party
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_ThreeParams(FOnCreatePartyComplete, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const ECreatePartyCompletionResult /*Result*/);
/**
 * Party update async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully updated party
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FourParams(FOnUpdatePartyComplete, const FUniqueNetId& /*LocalUserId*/, bool /*bWasSuccessful*/, const FOnlinePartyId& /*PartyId*/, const FString& /*Error*/);
/**
 * Party destroy async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully destroyed party
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FourParams(FOnDestroyPartyComplete, const FUniqueNetId& /*LocalUserId*/, bool /*bWasSuccessful*/, const FOnlinePartyId& /*PartyId*/, const FString& /*Error*/);
/**
 * Not Yet Implemented
 */
DECLARE_DELEGATE_FourParams(FOnRequestInvitationComplete, const FUniqueNetId& /*LocalUserId*/, bool /*bWasSuccessful*/, const FOnlinePartyId& /*PartyId*/, const FString& /*Error*/);
/**
 * Party join async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully joined party
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FourParams(FOnJoinPartyComplete, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const EJoinPartyCompletionResult /*Result*/, const int32 /*NotApprovedReason*/);
/**
 * Party leave async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully left party
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_ThreeParams(FOnLeavePartyComplete, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const ELeavePartyCompletionResult /*Result*/);
/**
 * Party invitation sent completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param PartyId - id associated with the party
 * @param Result - result of the send operation
 * @param RecipientId - user invite was sent to
 */
DECLARE_DELEGATE_FourParams(FOnSendInvitationComplete, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, ESendInviteCompleteResult /*Result*/, const FUniqueNetId& /*RecipientId*/);
/**
 * Accepting an invite to a user to join party async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully sent invite
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FourParams(FOnAcceptPartyInviteComplete, const FUniqueNetId& /*LocalUserId*/, bool /*bWasSuccessful*/, const FOnlinePartyId& /*PartyId*/, const FString& /*Error*/);
/**
 * Rejecting an invite to a user to join party async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully sent invite
 * @param PartyId - id associated with the party
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FourParams(FOnRejectPartyInviteComplete, const FUniqueNetId& /*LocalUserId*/, bool /*bWasSuccessful*/, const FOnlinePartyId& /*PartyId*/, const FString& /*Error*/);
/**
 * Kicking a member of a party async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully kicked member
 * @param PartyId - id associated with the party
 * @param MemberId - id of member being kicked
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FourParams(FOnKickPartyMemberComplete, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const EKickMemberCompletionResult /*Result*/);
/**
 * Promoting a member of a party async task completed callback
 *
 * @param LocalUserId - id of user that initiated the request
 * @param bWasSuccessful - true if successfully promoted member to admin
 * @param PartyId - id associated with the party
 * @param MemberId - id of member being promoted to admin
 * @param Error - string with error info if any
 */
DECLARE_DELEGATE_FiveParams(FOnPromotePartyMemberComplete, const FUniqueNetId& /*LocalUserId*/, bool /*bWasSuccessful*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const FString& /*Error*/);

// Notification delegates

/**
 * notification when a party is joined
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(F_PREFIX(OnPartyJoined), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyJoined);

/**
 * Notification when player promotion is locked out.
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param bLockoutState - if promotion is currently locked out
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(F_PREFIX(OnPartyPromotionLockoutChanged), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const bool /*bLockoutState*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyPromotionLockoutChanged);

/**
 * Notification when party data is updated
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param PartyConfig - party whose config was updated
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(F_PREFIX(OnPartyConfigChanged), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const TSharedRef<FPartyConfiguration>& /*PartyConfig*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyConfigChanged);

/**
 * Notification when party data is updated
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param PartyData - party data that was updated
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(F_PREFIX(OnPartyDataReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const TSharedRef<FOnlinePartyData>& /*PartyData*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyDataReceived);

/**
 * Notification when a member changes in a party
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param MemberId - id of member that joined
 * @param Reason - how the member changed
 */
DECLARE_MULTICAST_DELEGATE_FourParams(F_PREFIX(OnPartyMemberChanged), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const EMemberChangedReason /*Reason*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyMemberChanged);

/**
 * Notification when a member exits a party
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param MemberId - id of member that joined
 * @param Reason - why the member was removed
 */
DECLARE_MULTICAST_DELEGATE_FourParams(F_PREFIX(OnPartyMemberExited), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const EMemberExitedReason /*Reason*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyMemberExited);

/**
 * Notification when a member joins the party
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param MemberId - id of member that joined
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(F_PREFIX(OnPartyMemberJoined), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyMemberJoined);

/**
 * Notification when party member data is updated
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param MemberId - id of member that had updated data
 * @param PartyMemberData - party member data that was updated
 */
DECLARE_MULTICAST_DELEGATE_FourParams(F_PREFIX(OnPartyMemberDataReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const TSharedRef<FOnlinePartyData>& /*PartyMemberData*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyMemberDataReceived);

/**
 * Notification when an invite list has changed for a party
 * @param LocalUserId - user that is associated with this notification
 */
DECLARE_MULTICAST_DELEGATE_OneParam(F_PREFIX(OnPartyInvitesChanged), const FUniqueNetId& /*LocalUserId*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyInvitesChanged);

/**
 * Notification when a request for an invite has been received
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param SenderId - id of user that sent the invite
 * @param RequestForId - id of user that sender is requesting the invite for - invalid if the sender is requesting the invite
 */
DECLARE_MULTICAST_DELEGATE_FourParams(F_PREFIX(OnPartyInviteRequestReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/, const FUniqueNetId& /*RequestForId*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyInviteRequestReceived);

/**
 * Notification when a new invite is received
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param SenderId - id of member that sent the invite
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(F_PREFIX(OnPartyInviteReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyInviteReceived);

/**
 * Notification when a new invite is received
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param SenderId - id of member that sent the invite
 * @param bWasAccepted - whether or not the invite was accepted
 */
DECLARE_MULTICAST_DELEGATE_FourParams(F_PREFIX(OnPartyInviteResponseReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/, const EInvitationResponse /*bWasAccepted*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyInviteResponseReceived);

/**
 * Notification when a new reservation request is received
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param SenderId - id of member that sent the request
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(F_PREFIX(OnPartyJoinRequestReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyJoinRequestReceived);

/**
 * Notification when an invite is accepted
 * @param LocalUserId - id associated with this notification
 * @param PartyId - id associated with the party
 * @param MemberId - id of member that accepted the invite
 * @param bWasAccepted - whether or not the invite was accepted
 */
DECLARE_MULTICAST_DELEGATE_FourParams(F_PREFIX(OnPartyJoinRequestResponseReceived), const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, bool /*bWasAccepted*/);
PARTY_DECLARE_DELEGATETYPE(OnPartyJoinRequestResponseReceived);

/**
 * Interface definition for the online party services 
 * Allows for forming a party and communicating with party members
 */
class IOnlineParty
{
protected:
	IOnlineParty() {};

public:
	virtual ~IOnlineParty() {};

	/**
	 * Create a new party
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyConfig - configuration for the party (can be updated later)
	 * @param Delegate - called on completion
	 * @param UserRoomId - this forces the name of the room to be this value
	 *
	 * @return true if task was started
	 */
	virtual bool CreateParty(const FUniqueNetId& LocalUserId, const FPartyConfiguration& PartyConfig, const FOnCreatePartyComplete& Delegate = FOnCreatePartyComplete()) = 0;

	/**
	 * Update an existing party with new configuration
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyConfig - configuration for the party
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool UpdateParty(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FPartyConfiguration& PartyConfig, bool bShouldRegenerateReservationKey = false, const FOnUpdatePartyComplete& Delegate = FOnUpdatePartyComplete()) = 0;

	/**
	 * Destroy an existing party. Party members will be disbanded
	 * Only Owner/Admin can destroy the party
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool DestroyParty(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FOnDestroyPartyComplete& Delegate = FOnDestroyPartyComplete()) = 0;

	/**
	 * Join an existing party
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param RecipientId - id of the party leader
	 * @param ReservationKey - the ReservationKey for the party
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool JoinParty(const FUniqueNetId& LocalUserId, const IOnlinePartyJoinInfo& OnlinePartyJoinInfo, const FOnJoinPartyComplete& Delegate = FOnJoinPartyComplete()) = 0;

	/**
	 * Leave an existing party
	 * All existing party members notified of member leaving (see FOnPartyMemberLeft)
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool LeaveParty(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FOnLeavePartyComplete& Delegate = FOnLeavePartyComplete()) = 0;

	/**
	* Approve a request to join a party
	*
	* @param LocalUserId - user making the request
	* @param PartyId - id of an existing party
	* @param RecipientId - id of the user being invited
	* @param DeniedResultCode - client defined value to return when leader denies approval
	*
	* @return true if task was started
	*/
	virtual bool ApproveJoinRequest(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& RecipientId, bool bIsApproved, int32 DeniedResultCode = 0) = 0;

	/**
	* sends an request to a party leader for an invite to a party that could not otherwise be joined
	* if the player accepts the invite they will be sent the data needed to trigger a call to RequestReservation
	*
	* @param LocalUserId - user making the request
	* @param PartyId - id of an existing party
	* @param LeaderId - id of the user to request an invitation from
	* @param RequestForId - id of the user you are requesting an invite for
	* @param Delegate - called on completion
	*
	* @return true if task was started
	*/
	virtual bool RequestInvitation(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& LeaderId, const FUniqueNetId& RequestForId, const FOnRequestInvitationComplete& Delegate = FOnRequestInvitationComplete()) = 0;

	/**
	 * sends an invitation to a user that could not otherwise join a party
	 * if the player accepts the invite they will be sent the data needed to trigger a call to RequestReservation
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param RecipientId - id of the user being invited
	 * @param CustomPayload - game defined key value pairs
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool SendInvitation(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& RecipientId, const FOnlinePartyData& CustomPayload = FOnlinePartyData(), const FOnSendInvitationComplete& Delegate = FOnSendInvitationComplete()) = 0;

	/**
	* Accept an invite to a party. NOTE this does not initiate a join.
	*
	* @param LocalUserId - user making the request
	* @param PartyId - id of an existing party
	* @param Delegate - called on completion
	*
	* @return true if task was started
	*/
	virtual bool AcceptInvitation(const FUniqueNetId& LocalUserId, const FUniqueNetId& SenderId) = 0;

	/**
	* Reject an invite to a party
	*
	* @param LocalUserId - user making the request
	* @param PartyId - id of an existing party
	*
	* @return true if task was started
	*/
	virtual bool RejectInvitation(const FUniqueNetId& LocalUserId, const FUniqueNetId& SenderId) = 0;

	/**
	 * Kick a user from an existing party
	 * Only admin can kick a party member
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param MemberId - id of the user being kicked
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool KickMember(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& MemberId, const FOnKickPartyMemberComplete& Delegate = FOnKickPartyMemberComplete()) = 0;

	/**
	 * Promote a user from an existing party to be admin
	 * All existing party members notified of promoted member (see FOnPartyMemberPromoted)
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param MemberId - id of the user being promoted
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool PromoteMember(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& MemberId, const FOnPromotePartyMemberComplete& Delegate = FOnPromotePartyMemberComplete()) = 0;

	/**
	 * Set party data and broadcast to all members
	 * Only current data can be set and no history of past party data is preserved
	 * Party members notified of new data (see FOnPartyDataReceived)
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param PartyData - data to send to all party members
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool UpdatePartyData(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FOnlinePartyAttrs& PartyData) = 0;

	/**
	 * Set party data for a single party member and broadcast to all members
	 * Only current data can be set and no history of past party member data is preserved
	 * Party members notified of new data (see FOnPartyMemberDataReceived)
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId - id of an existing party
	 * @param PartyMemberData - member data to send to all party members
	 * @param Delegate - called on completion
	 *
	 * @return true if task was started
	 */
	virtual bool UpdatePartyMemberData(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FOnlinePartyAttrs& PartyMemberData) = 0;

	/**
	 * returns true if the user specified by MemberId is the leader of the party specified by PartyId
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId     - id of an existing party
	 * @param MemberId    - id of member to test
	 *
	 */
	virtual bool IsMemberLeader(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& MemberId) const = 0;

	/**
	 * returns the number of players in a given party
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId     - id of an existing party
	 *
	 */
	virtual uint32 GetPartyMemberCount(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId) const = 0;

	/**
	 * Get info associated with a party
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId     - id of an existing party
	 *
	 * @return party info or nullptr if not found
	 */
	virtual TSharedPtr<FOnlinePartyInfo> GetPartyInfo(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId) const = 0;

	/**
	 * Get a party member by id
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId     - id of an existing party
	 * @param MemberId    - id of member to find
	 *
	 * @return party member info or nullptr if not found
	 */
	virtual TSharedPtr<FOnlinePartyMember> GetPartyMember(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& MemberId) const = 0;

	/**
	 * Get current cached data associated with a party
	 * FOnPartyDataReceived notification called whenever this data changes
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId     - id of an existing party
	 *
	 * @return party data or nullptr if not found
	 */
	virtual TSharedPtr<FOnlinePartyData> GetPartyData(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId) const = 0;

	/**
	 * Get current cached data associated with a party member
	 * FOnPartyMemberDataReceived notification called whenever this data changes
	 *
	 * @param LocalUserId - user making the request
	 * @param PartyId     - id of an existing party
	 * @param MemberId    - id of member to find data for
	 *
	 * @return party member data or nullptr if not found
	 */
	virtual TSharedPtr<FOnlinePartyData> GetPartyMemberData(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& MemberId) const = 0;

	/**
	 * returns true if the user is advertising a party on their presence
	 *
	 * @param LocalUserId       - user making the request
	 * @param UserId            - user to check
	 * @param OutPartyIdString  - string id of advertised party
	 * @param OutLeaderIdString - string id of advertised party's leader
	 * @param OutReservationKey - (optional) reservation key of the advertised party
	 *
	 */
	virtual TSharedPtr<IOnlinePartyJoinInfo> GetAdvertisedParty(const FUniqueNetId& LocalUserId, const FUniqueNetId& UserId) = 0;

	/**
	 * Get a list of currently joined parties for the user
	 *
	 * @param LocalUserId     - user making the request
	 * @param OutPartyIdArray - list of party ids joined by the current user
	 *
	 * @return true if entries found
	 */
	virtual bool GetJoinedParties(const FUniqueNetId& LocalUserId, TArray<TSharedRef<const FOnlinePartyId>>& OutPartyIdArray) const = 0;

	/**
	 * Get list of current party members
	 *
	 * @param LocalUserId          - user making the request
	 * @param PartyId              - id of an existing party
	 * @param OutPartyMembersArray - list of party members currently in the party
	 *
	 * @return true if entries found
	 */
	virtual bool GetPartyMembers(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, TArray<TSharedRef<FOnlinePartyMember>>& OutPartyMembersArray) const = 0;

	/**
	 * Get a list of parties the user has been invited to
	 *
	 * @param LocalUserId            - user making the request
	 * @param OutPendingInvitesArray - list of party info needed to join the party
	 *
	 * @return true if entries found
	 */
	virtual bool GetPendingInvites(const FUniqueNetId& LocalUserId, TArray<TSharedRef<IOnlinePartyInvite>>& OutPendingInvitesArray) const = 0;

	/**
	 * Get list of users requesting to join the party
	 *
	 * @param LocalUserId           - user making the request
	 * @param PartyId               - id of an existing party
	 * @param OutPendingUserIdArray - list of pending party members
	 *
	 * @return true if entries found
	 */
	virtual bool GetPendingJoinRequests(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, TArray<TSharedRef<IOnlinePartyPendingJoinRequestInfo>>& OutPendingJoinRequestArray) const = 0;

	/**
	 * Get list of users invited to a party that have not yet responded
	 *
	 * @param LocalUserId                - user making the request
	 * @param PartyId                    - id of an existing party
	 * @param OutPendingInvitedUserArray - list of user that have pending invites
	 *
	 * @return true if entries found
	 */
	virtual bool GetPendingInvitedUsers(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, TArray<TSharedRef<const FUniqueNetId>>& OutPendingInvitedUserArray) const = 0;

	/**
	 * Creates a command line token from a IOnlinePartyJoinInfo object
	 *
	 * @param JoinInfo - the IOnlinePartyJoinInfo object to convert
	 *
	 * return the new IOnlinePartyJoinInfo object
	 */
	virtual FString MakeTokenFromJoinInfo(const IOnlinePartyJoinInfo& JoinInfo) const = 0;

	/**
	 * Creates a IOnlinePartyJoinInfo object from a command line token
	 *
	 * @param Token - the token string
	 *
	 * return the new IOnlinePartyJoinInfo object
	 */
	virtual TSharedRef<IOnlinePartyJoinInfo> MakeJoinInfoFromToken(const FString& Token) const = 0;

	/**
	 * Checks to see if there is a pending command line invite and consumes it
	 *
	 * return the pending IOnlinePartyJoinInfo object
	 */
	virtual TSharedPtr<IOnlinePartyJoinInfo> ConsumePendingCommandLineInvite() = 0;

	/**
	 * List of all subscribe-able notifications
	 *
	 * OnPartyJoined
	 * OnPartyPromotionLockoutStateChanged
	 * OnPartyConfigChanged
	 * OnPartyDataChanged
	 * OnPartyMemberChanged
	 * OnPartyMemberExited
	 * OnPartyMemberJoined
	 * OnPartyMemberDataChanged
	 * OnPartyInvitesChanged
	 * OnPartyInviteRequestReceived
	 * OnPartyInviteReceived
	 * OnPartyInviteResponseReceived
	 * OnPartyJoinRequestReceived
	 * OnPartyJoinRequestResponseReceived
	 *
	 */

	/**
	 * notification of when a party is joined
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 */
	DEFINE_ONLINE_DELEGATE_TWO_PARAM(OnPartyJoined, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/);

	/**
	 * Notification when player promotion is locked out.
	 *
	 * @param PartyId - id associated with the party
	 * @param bLockoutState - if promotion is currently locked out
	 */
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnPartyPromotionLockoutChanged, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const bool /*bLockoutState*/);

	/**
	 * Notification when party data is updated
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param PartyConfig - party whose config was updated
	 */
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnPartyConfigChanged, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const TSharedRef<FPartyConfiguration>& /*PartyConfig*/);

	/**
	 * Notification when party data is updated
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param PartyData - party data that was updated
	 */
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnPartyDataReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const TSharedRef<FOnlinePartyData>& /*PartyData*/);

	/**
	 * Notification when a member changes in a party
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param MemberId - id of member that joined
	 * @param Reason - how the member changed
	 */
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnPartyMemberChanged, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const EMemberChangedReason /*Reason*/);

	/**
	 * Notification when a member exits a party
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param MemberId - id of member that joined
	 * @param Reason - why the member was removed
	 */
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnPartyMemberExited, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const EMemberExitedReason /*Reason*/);

	/**
	 * Notification when a member joins the party
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param MemberId - id of member that joined
	 */
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnPartyMemberJoined, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/);

	/**
	 * Notification when party member data is updated
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param MemberId - id of member that had updated data
	 * @param PartyMemberData - party member data that was updated
	 */
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnPartyMemberDataReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, const TSharedRef<FOnlinePartyData>& /*PartyMemberData*/);

	/**
	 * Notification when an invite list has changed for a party
	 * @param LocalUserId - user that is associated with this notification
	 */
	DEFINE_ONLINE_DELEGATE_ONE_PARAM(OnPartyInvitesChanged, const FUniqueNetId& /*LocalUserId*/);

	/**
	 * Notification when a request for an invite has been received
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param SenderId - id of user that sent the invite
	 * @param RequestForId - id of user that sender is requesting the invite for - invalid if the sender is requesting the invite
	 */
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnPartyInviteRequestReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/, const FUniqueNetId& /*RequestForId*/);

	/**
	 * Notification when a new invite is received
	 * @param LocalUserId - id associated with this notification
	 * @param SenderId - id of member that sent the invite
	 * @param PartyId - id associated with the party
	 */
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnPartyInviteReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/);

	/**
	 * Notification when a new invite is received
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param SenderId - id of member that sent the invite
	 * @param bWasAccepted - true is the invite was accepted
	 */
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnPartyInviteResponseReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/, const EInvitationResponse /*bWasAccepted*/);

	/**
	 * Notification when a new reservation request is received
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param SenderId - id of member that sent the request
	 */
	DEFINE_ONLINE_DELEGATE_THREE_PARAM(OnPartyJoinRequestReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*SenderId*/);

	/**
	 * Notification when a join request is approved
	 * @param LocalUserId - id associated with this notification
	 * @param PartyId - id associated with the party
	 * @param MemberId - id of member that accepted the invite
	 * @param bWasAccepted - whether or not the invite was accepted
	 */
	DEFINE_ONLINE_DELEGATE_FOUR_PARAM(OnPartyJoinRequestResponseReceived, const FUniqueNetId& /*LocalUserId*/, const FOnlinePartyId& /*PartyId*/, const FUniqueNetId& /*MemberId*/, bool /*bWasAccepted*/);

	/**
	 * Dump out party state for all known parties
	 */
	virtual void DumpPartyState() = 0;

};

typedef TSharedPtr<IOnlineParty, ESPMode::ThreadSafe> IOnlinePartyPtr;

enum class EInvitationResponse
{
	UnknownFailure,
	BadBuild,
	Rejected,
	Accepted
};

inline const TCHAR* ToString(EInvitationResponse Value)
{
	switch (Value)
	{
	case EInvitationResponse::UnknownFailure:
	{
		return TEXT("UnknownFailure");
	}
	case EInvitationResponse::BadBuild:
	{
		return TEXT("BadBuild");
	}
	case EInvitationResponse::Rejected:
	{
		return TEXT("Rejected");
	}
	case EInvitationResponse::Accepted:
	{
		return TEXT("Accepted");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(EMemberChangedReason Value)
{
	switch (Value)
	{
	case EMemberChangedReason::Disconnected:
	{
		return TEXT("Disconnected");
	}
	case EMemberChangedReason::Rejoined:
	{
		return TEXT("Rejoined");
	}
	case EMemberChangedReason::Promoted:
	{
		return TEXT("Promoted");
	}
	}
	return TEXT("");
}

/** @return the stringified version of the enum passed in */
inline const TCHAR* ToString(EPartyState Value)
{
	switch (Value)
	{
	case EPartyState::None:
	{
		return TEXT("None");
	}
	case EPartyState::CreatePending:
	{
		return TEXT("Create pending");
	}
	case EPartyState::DestroyPending:
	{
		return TEXT("Destroy pending");
	}
	case EPartyState::JoinPending:
	{
		return TEXT("Join pending");
	}
	case EPartyState::JoinChatPending:
	{
		return TEXT("Join Chat pending");
	}
	case EPartyState::LeavePending:
	{
		return TEXT("Leave pending");
	}
	case EPartyState::RemovalPending:
	{
		return TEXT("Removal pending");
	}
	case EPartyState::Active:
	{
		return TEXT("Active");
	}
	case EPartyState::Disconnected:
	{
		return TEXT("Disconnected");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(EMemberExitedReason Value)
{
	switch (Value)
	{
	case EMemberExitedReason::Unknown:
	{
		return TEXT("Unknown");
	}
	case EMemberExitedReason::Left:
	{
		return TEXT("Left");
	}
	case EMemberExitedReason::Removed:
	{
		return TEXT("Removed");
	}
	case EMemberExitedReason::Disbanded:
	{
		return TEXT("Disbanded");
	}
	case EMemberExitedReason::Kicked:
	{
		return TEXT("Kicked");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(ECreatePartyCompletionResult Value)
{
	switch (Value)
	{
	case ECreatePartyCompletionResult::UnknownClientFailure:
	{
		return TEXT("UnknownClientFailure");
	}
	case ECreatePartyCompletionResult::AlreadyCreatingParty:
	{
		return TEXT("AlreadyCreatingParty");
	}
	case ECreatePartyCompletionResult::AlreadyInParty:
	{
		return TEXT("AlreadyInParty");
	}
	case ECreatePartyCompletionResult::FailedToCreateMucRoom:
	{
		return TEXT("FailedToCreateMucRoom");
	}
	case ECreatePartyCompletionResult::NoResponse:
	{
		return TEXT("NoResponse");
	}
	case ECreatePartyCompletionResult::UnknownInternalFailure:
	{
		return TEXT("UnknownInternalFailure");
	}
	case ECreatePartyCompletionResult::Succeeded:
	{
		return TEXT("Succeeded");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(ESendInviteCompleteResult Value)
{
	switch (Value)
	{
	case ESendInviteCompleteResult::NotLoggedIn:
	{
		return TEXT("NotLoggedIn");
	}
	case ESendInviteCompleteResult::InvitePending:
	{
		return TEXT("InvitePending");
	}
	case ESendInviteCompleteResult::AlreadyInParty:
	{
		return TEXT("AlreadyInParty");
	}
	case ESendInviteCompleteResult::PartyFull:
	{
		return TEXT("PartyFull");
	}
	case ESendInviteCompleteResult::NoPermission:
	{
		return TEXT("NoPermission");
	}
	case ESendInviteCompleteResult::UnknownInternalFailure:
	{
		return TEXT("UnknownInternalFailure");
	}
	case ESendInviteCompleteResult::Succeeded:
	{
		return TEXT("Succeeded");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(EJoinPartyCompletionResult Value)
{
	switch (Value)
	{
	case EJoinPartyCompletionResult::UnknownClientFailure:
	{
		return TEXT("UnknownClientFailure");
	}
	case EJoinPartyCompletionResult::JoinInfoInvalid:
	{
		return TEXT("JoinInfoInvalid");
	}
	case EJoinPartyCompletionResult::AlreadyJoiningParty:
	{
		return TEXT("AlreadyJoiningParty");
	}
	case EJoinPartyCompletionResult::AlreadyInParty:
	{
		return TEXT("AlreadyInParty");
	}
	case EJoinPartyCompletionResult::UnknownLocalUser:
	{
		return TEXT("UnknownLocalUser");
	}
	case EJoinPartyCompletionResult::FailedToJoinChatRoom:
	{
		return TEXT("FailedToJoinChatRoom");
	}
	case EJoinPartyCompletionResult::MessagingFailure:
	{
		return TEXT("MessagingFailure");
	}
	case EJoinPartyCompletionResult::NoSpace:
	{
		return TEXT("NoSpace");
	}
	case EJoinPartyCompletionResult::NotApproved:
	{
		return TEXT("NotApproved");
	}
	case EJoinPartyCompletionResult::RequesteeNotMember:
	{
		return TEXT("RequesteeNotMember");
	}
	case EJoinPartyCompletionResult::RequesteeNotLeader:
	{
		return TEXT("RequesteeNotLeader");
	}
	case EJoinPartyCompletionResult::NoResponse:
	{
		return TEXT("NoResponse");
	}
	case EJoinPartyCompletionResult::UnknownTransportFailure:
	{
		return TEXT("UnknownTransportFailure");
	}
	case EJoinPartyCompletionResult::UnknownInternalFailure:
	{
		return TEXT("UnknownInternalFailure");
	}
	case EJoinPartyCompletionResult::Succeeded:
	{
		return TEXT("Succeeded");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(ELeavePartyCompletionResult Value)
{
	switch (Value)
	{
	case ELeavePartyCompletionResult::UnknownClientFailure:
	{
		return TEXT("UnknownClientFailure");
	}
	case ELeavePartyCompletionResult::AlreadyLeavingParty:
	{
		return TEXT("AlreadyLeavingParty");
	}
	case ELeavePartyCompletionResult::UnknownLocalUser:
	{
		return TEXT("UnknownLocalUser");
	}
	case ELeavePartyCompletionResult::UnknownParty:
	{
		return TEXT("UnknownParty");
	}
	case ELeavePartyCompletionResult::MessagingFailure:
	{
		return TEXT("MessagingFailure");
	}
	case ELeavePartyCompletionResult::NoResponse:
	{
		return TEXT("NoResponse");
	}
	case ELeavePartyCompletionResult::UnknownTransportFailure:
	{
		return TEXT("UnknownTransportFailure");
	}
	case ELeavePartyCompletionResult::UnknownInternalFailure:
	{
		return TEXT("UnknownInternalFailure");
	}
	case ELeavePartyCompletionResult::Succeeded:
	{
		return TEXT("Succeeded");
	}
	}
	return TEXT("");
}

inline const TCHAR* ToString(EKickMemberCompletionResult Value)
{
	switch (Value)
	{
	case EKickMemberCompletionResult::UnknownClientFailure:
	{
		return TEXT("UnknownClientFailure");
	}
	case EKickMemberCompletionResult::UnknownParty:
	{
		return TEXT("UnknownParty");
	}
	case EKickMemberCompletionResult::LocalMemberNotMember:
	{
		return TEXT("LocalMemberNotMember");
	}
	case EKickMemberCompletionResult::LocalMemberNotLeader:
	{
		return TEXT("LocalMemberNotLeader");
	}
	case EKickMemberCompletionResult::RemoteMemberNotMember:
	{
		return TEXT("RemoteMemberNotMember");
	}
	case EKickMemberCompletionResult::MessagingFailure:
	{
		return TEXT("MessagingFailure");
	}
	case EKickMemberCompletionResult::NoResponse:
	{
		return TEXT("NoResponse");
	}
	case EKickMemberCompletionResult::UnknownInternalFailure:
	{
		return TEXT("UnknownInternalFailure");
	}
	case EKickMemberCompletionResult::Succeeded:
	{
		return TEXT("Succeeded");
	}
	}
	return TEXT("");
}

inline void FOnlinePartyData::ToJson(FString& JsonString) const
{
	JsonString.Empty();

	// iterate over key/val attrs and convert each entry to a json string
	TSharedRef<FJsonObject> JsonObject(new FJsonObject());
	TArray<TSharedPtr<FJsonValue> > JsonProperties;
	for (FOnlinePartyAttrs::TConstIterator It(KeyValAttrs); It; ++It)
	{
		const FString& PropertyName = It.Key();
		const FVariantData& PropertyValue = It.Value();

		TSharedRef<FJsonObject> PropertyJson = PropertyValue.ToJson();
		PropertyJson->SetStringField(TEXT("Name"), *PropertyName);
		JsonProperties.Add(MakeShareable(new FJsonValueObject(PropertyJson)));
	}
	JsonObject->SetArrayField(TEXT("Attrs"), JsonProperties);

	auto JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject, JsonWriter);
	JsonWriter->Close();
}

inline void FOnlinePartyData::FromJson(const FString& JsonString)
{
	KeyValAttrs.Empty();

	// json string to key/val attrs
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) &&
		JsonObject.IsValid())
	{
		if (JsonObject->HasTypedField<EJson::Array>(TEXT("Attrs")))
		{
			const TArray<TSharedPtr<FJsonValue> >& JsonProperties = JsonObject->GetArrayField(TEXT("Attrs"));
			for (auto JsonPropertyValue : JsonProperties)
			{
				TSharedPtr<FJsonObject> JsonPropertyObject = JsonPropertyValue->AsObject();
				if (JsonPropertyObject.IsValid())
				{
					FString PropertyName;
					if (JsonPropertyObject->TryGetStringField(TEXT("Name"), PropertyName) &&
						!PropertyName.IsEmpty())
					{
						FVariantData PropertyData;
						if (PropertyData.FromJson(JsonPropertyObject.ToSharedRef()))
						{
							KeyValAttrs.Add(PropertyName, PropertyData);
						}
					}
				}
			}
		}
	}
}