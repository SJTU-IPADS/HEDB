
# CFG_TA_MCOUNT ?= y

CFG_TEE_TA_LOG_LEVEL ?= 4
CFG_TEE_CORE_LOG_LEVEL ?= 4 
CFG_PAGED_USER_TA = y

CPPFLAGS += -DCFG_TEE_TA_LOG_LEVEL=$(CFG_TEE_TA_LOG_LEVEL)

TA_DEV_KIT_DIR ?= $(PWD)/ta_dev_kit

# The UUID for the Trusted Application
BINARY=64974d40-92cd-4714-9e8f-442f3a4db3af

-include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk

ifeq ($(wildcard $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk), )
clean:
	@echo 'Note: $$(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk not found, cannot clean TA'
	@echo 'Note: TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)'
endif
