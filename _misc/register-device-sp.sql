USE [home-automation]
GO
/****** Object:  StoredProcedure [dbo].[REGISTER_DEVICE]    Script Date: 2024-07-12 9:58:31 PM ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		<Nicholas Biancolin>
-- Create date: <July 7th, 2024>
-- Description:	<Called by an arduino the first time it powers up>
-- =============================================
ALTER PROCEDURE [dbo].[REGISTER_DEVICE]
	@DISPLAY_NAME varchar(64),
	@DEVICE_TYPE varchar(64),
	@ADDRESS varchar(16)
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

    -- Check if a device with the given "display name" exists already
	IF EXISTS (SELECT 1 FROM dbo.devices WHERE [ADDRESS] = @ADDRESS)
	BEGIN
		-- give that device its new IP
		UPDATE dbo.devices SET DISPLAY_NAME = @DISPLAY_NAME, DEVICE_TYPE = @DEVICE_TYPE, ACTIVE = 'true' WHERE [ADDRESS] = @ADDRESS;
	END
	ELSE
	BEGIN
		INSERT INTO dbo.devices (DISPLAY_NAME, DEVICE_TYPE, [ADDRESS], ACTIVE) VALUES (@DISPLAY_NAME, @DEVICE_TYPE, @ADDRESS, 'true');
	END
	
	SELECT SCOPE_IDENTITY();
END
