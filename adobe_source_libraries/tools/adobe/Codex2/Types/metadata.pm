# =========================================================================
# Formatting: Expand tabs, 4 spaces per indent level.
# =========================================================================

=pod

=head1 NAME

Adobe::Codex2::Types::metadata - Define Codex type data.

=head1 DESCRIPTION

See POD from Adobe::Codex2.pm for details on using the Codex modules.

=head2 XML

  <xs:element name="metadata">
    <xs:complexType>
      <xs:sequence>
        <xs:element maxOccurs="unbounded" minOccurs="0" ref="schemas:item"/>
      </xs:sequence>
    </xs:complexType>
  </xs:element>

=head1 SUBROUTINES/METHODS

=cut

# -------------------------------------------------------------------------
# Module setup.
# -------------------------------------------------------------------------
package Adobe::Codex2::Types::metadata;

require 5.8.0;
use strict;
use warnings;

use base qw(Exporter);
our $VERSION     = "0.2.0";
our @ISA         = qw();
our @EXPORT      = qw();
our @EXPORT_OK   = qw(&Data);
our %EXPORT_TAGS = ();

# Get Perl modules.
# None.

# Get Codex modules.
use Adobe::Codex2::Types::item;

# -------------------------------------------------------------------------
# Data
# -------------------------------------------------------------------------
sub Data
{
    # Get arguments.
    my $Data = shift;

    # Check arguments.
    # TODO
    
    # Check data.
    # TODO

    # Storage for results.
    my @metadata;

    # Process data.
    if (exists $Data->{'item'})
    {
        # Single item.
        if (ref($Data->{'item'}) eq "HASH")
        {
            my %item = Adobe::Codex2::Types::item::Data($Data->{'item'});
            push(@metadata, \%item);
        }
        # Multiple items.
        elsif (ref($Data->{'item'}) eq "ARRAY")
        {
            foreach my $itemData (@{$Data->{'item'}})
            {
                my %item = Adobe::Codex2::Types::item::Data($itemData);
                push(@metadata, \%item);
            }
        }
    }

    # Return results.
    return @metadata;
}

1;
__END__

=pod

=head1 AUTHOR

    Dave Foglesong (fogleson@adobe.com)

=head1 COPYRIGHT

    Copyright 2008 Adobe Systems Incorporated. All rights reserved.

=cut

