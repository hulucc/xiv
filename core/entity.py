from ctypes.wintypes import c_float
from ctypes.wintypes import c_ubyte
from ctypes.wintypes import c_uint
from ctypes.wintypes import create_string_buffer
from munch import Munch
import logging


from core.offsets import entity_base_offsets
from core.registerable import Registerable
from core.structs import Parameters
from core.structs import Position
from lib.memory import RegistryEntry

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)

MAX_NAME_LENGTH = 32
NAME_BUFFER = create_string_buffer('_' * MAX_NAME_LENGTH)


class Entity(Registerable):

    def __init__(self, address):
        self.name = None
        self.level = None
        self.address = address
        self.parameters = Parameters()
        self.position = Position()

        super(Entity, self).__init__()

    def __repr__(self):
        return str({
            'name': self.name,
            'level': self.level,
            'address': hex(self.address),
            'parameters': self.parameters,
            'position': self.position,
        })

    def _update_registry_map(self):
        # Must be called here as pointers are likely already resolved
        # For the entity, everything will be built on the base address.
        self.REGISTRY_MAP = Munch(
            current_hp=self._build_registry_entry(
                'entity base', 'current_hp', 'parameters current_hp'
            ),
            max_hp=self._build_registry_entry(
                'entity base', 'max_hp', 'parameters max_hp'
            ),
            current_mp=self._build_registry_entry(
                'entity base', 'current_mp', 'parameters current_mp'
            ),
            max_mp=self._build_registry_entry(
                'entity base', 'max_mp', 'parameters max_mp'
            ),

            name=self._build_registry_entry(
                'entity base', 'name', data_type=NAME_BUFFER
            ),
            level=self._build_registry_entry(
                'entity base', 'level', 'level', data_type=c_ubyte()
            ),
            x=self._build_registry_entry('entity base', 'x', 'position x', c_float()),
            y=self._build_registry_entry('entity base', 'y', 'position y', c_float()),
            z=self._build_registry_entry('entity base', 'z', 'position z', c_float()),
        )

    def _build_registry_entry(
        self, description, offset_name,
        attribute_path_str=None, data_type=c_uint()
    ):
        """
        description -> determines which address and offsets are used.
        offset_name -> must match property name in corresponding OFFSETS.
        attribute_path_str -> how the value is to be assigned to the class.
            e.g. Player.position.x: 'position x'
        data_type -> specified wintype necessary for reading correct amount of bytes
        """

        if not attribute_path_str:
            attribute_path_str = offset_name

        address = self.address
        offset = entity_base_offsets[offset_name]

        return RegistryEntry(
            reference=self, description=description, attribute_path=attribute_path_str.split(),
            address=address, offset=offset, data_type=data_type
        )


