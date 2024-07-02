"""Speaker platform implementation as ADF-Pipeline Element."""

import esphome.codegen as cg
from esphome.components import speaker
import esphome.config_validation as cv
from esphome.const import CONF_ID

from .. import (
    esp_adf_ns,
    ADFPipelineController,
    ADF_PIPELINE_CONTROLLER_SCHEMA,
    setup_pipeline_controller,
)


CODEOWNERS = ["@gnumpi"]
DEPENDENCIES = ["adf_pipeline"]


ADFMixer = esp_adf_ns.class_(
    "ADFMixer", ADFPipelineController, cg.Component
)
CONF_INPUT_PIPELINES = "input_pipelines"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ADFMixer),
        cv.Required(CONF_INPUT_PIPELINES): cv.ensure_list(
            cv.Any(
                cv.use_id(ADFPipelineController),
            )
        ),
    }
).extend(ADF_PIPELINE_CONTROLLER_SCHEMA)


# @coroutine_with_priority(100.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_input_pipelines(config[CONF_INPUT_PIPELINES]))
    await cg.register_component(var, config)
    await setup_pipeline_controller(var, config)
   # await speaker.register_speaker(var, config)
